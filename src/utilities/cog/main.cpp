#include "program/program.hpp"
#include "jk/cog/compiler/compiler.hpp"
#include "jk/cog/compiler/script_loader.hpp"
#include "io/native_file.hpp"
#include "jk/cog/vm/executor.hpp"
#include "jk/cog/vm/default_verbs.hpp"
#include "utility/zip.hpp"
#include "utility/time.hpp"
#include "scenario.hpp"
#include "vfs/native_file_system.hpp"
#include "content/content_manager.hpp"
#include "content/loader_registry.hpp"
#include "value_mapping.hpp"
#include "io/native_file.hpp"
#include <vector>
#include <unordered_map>
#include <iostream>

namespace gorc {

    class cog_scenario_state {
    public:
        service_registry services;
        std::unique_ptr<content_manager> content;
        std::unique_ptr<cog::executor> executor;
        time_delta current_time = 0.0s;

        cog_scenario_state(cog_scenario &scn,
                           service_registry const &parent_services)
            : services(&parent_services)
        {
            content = std::make_unique<content_manager>(services);
            services.add(*content);

            executor = std::make_unique<cog::executor>(services);
            for(auto const &file : scn.cog_files) {
                auto script = content->load<cog::script>(file.cog_filename);
                cog_id instance_id;
                if(file.init.empty()) {
                    instance_id = executor->create_instance(script);
                }
                else {
                    instance_id = executor->create_instance(script, file.init);
                }
            }
        }

        cog_scenario_state(deserialization_constructor_tag,
                           input_stream &is,
                           service_registry const &parent_services)
            : services(&parent_services)
        {
            binary_input_stream bis(is, services);

            content = std::make_unique<content_manager>(deserialization_constructor, bis);
            services.add(*content);

            executor = std::make_unique<cog::executor>(deserialization_constructor, bis);

            current_time = binary_deserialize<time_delta>(bis);
        }

        void binary_serialize_object(output_stream &os) const
        {
            binary_output_stream bos(os, services);

            binary_serialize(bos, *content);
            binary_serialize(bos, *executor);
            binary_serialize(bos, current_time);
        }
    };

    class cog_program : public program, public cog_scenario_event_visitor {
    private:
        service_registry services;

        std::string scenario_file;
        cog::verb_table verbs;

        std::unique_ptr<cog_scenario_state> state;

        std::unordered_map<std::string, std::unique_ptr<memory_file>> quicksaves;

    public:
        virtual void create_options(options &opts) override
        {
            opts.insert(make_value_option("scenario", scenario_file));
            opts.emplace_constraint<required_option>("scenario");
        }

        virtual int run() override
        {
            cog::constant_table constants;
            cog::default_populate_constant_table(constants);

            cog::default_populate_verb_table(verbs);
            populate_verb_table();
            services.add(verbs);

            loader_registry loaders;
            loaders.emplace_loader<cog::script_loader>();
            services.add(loaders);

            cog::compiler compiler(verbs, constants);
            services.add(compiler);

            native_file_system vfs;
            services.add<virtual_file_system>(vfs);

            // Load scenario file
            diagnostic_context dc(scenario_file.c_str());
            auto f = make_native_read_only_file(scenario_file);
            json_input_stream jis(*f);
            cog_scenario scenario(deserialization_constructor, jis);

            cog_scenario_value_mapping val_map(scenario);
            services.add<cog::default_value_mapping>(val_map);

            // Construct instances:
            state = std::make_unique<cog_scenario_state>(scenario, services);

            // Execute startup messages:
            state->executor->send_to_all(cog::message_type::startup,
                                         /* sender: nothing */ 0,
                                         /* senderid: nothing */ cog::value(),
                                         /* source: nothing */ 0,
                                         /* param0 */ cog::value(),
                                         /* param1 */ cog::value(),
                                         /* param2 */ cog::value(),
                                         /* param3 */ cog::value());

            for(auto const &event : scenario.events) {
                event->accept(*this);
            }

            return EXIT_SUCCESS;
        }

        virtual void visit(time_step_event const &e) override
        {
            state->current_time += e.step;
            std::cout << "T+" << state->current_time.count() << std::endl;
            state->executor->update(e.step);
        }

        virtual void visit(send_linked_event const &e) override
        {
            state->executor->send_to_linked(e.msg,
                                            e.sender,
                                            e.source,
                                            e.st,
                                            e.param0,
                                            e.param1,
                                            e.param2,
                                            e.param3);
        }

        virtual void visit(quicksave_event const &e) override
        {
            auto mf = std::make_unique<memory_file>();
            state->binary_serialize_object(*mf);

            quicksaves.erase(e.key);
            quicksaves.emplace(e.key, std::move(mf));

            std::cout << "SAVE: " << e.key << std::endl;
        }

        virtual void visit(quickload_event const &e) override
        {
            memory_file::reader mr(*quicksaves.at(e.key));
            state = std::make_unique<cog_scenario_state>(deserialization_constructor,
                                                         mr,
                                                         services);
            std::cout << "LOAD: " << e.key << std::endl;
        }

        void populate_verb_table()
        {
            verbs.add_verb("print", [](char const *s) {
                    std::cout << s << std::endl;
                });

            verbs.add_verb("printint", [](int v) {
                    std::cout << v << std::endl;
                });

            class printvar_verb : public cog::verb {
            public:
                printvar_verb(std::string const &name)
                    : verb(name,
                           cog::value_type::nothing,
                           std::vector<cog::value_type> { cog::value_type::dynamic })
                {
                    return;
                }

                virtual cog::value invoke(cog::stack &stk, service_registry &, bool) const override
                {
                    std::cout << cog::as_string(stk.back()) << std::endl;
                    stk.pop_back();
                    return cog::value();
                }
            };

            verbs.emplace_verb<printvar_verb>("printvar");

            verbs.add_verb("getglobalcog", [&](char const *cogname) {
                    return state->executor->create_global_instance(
                            state->content->load<cog::script>(cogname));
                });

            // Test verbs for typesafe casting
            verbs.add_safe_verb("assert_sector", cog::value(), [](sector_id) { });
            verbs.add_safe_verb("assert_surface", cog::value(), [](surface_id) { });
            verbs.add_safe_verb("assert_thing", cog::value(), [](thing_id) { });
            verbs.add_safe_verb("assert_player", cog::value(), [](player_id) { });
            verbs.add_safe_verb("assert_ai", cog::value(), [](ai_id) { });
            verbs.add_safe_verb("assert_cog", cog::value(), [](cog_id) { });
            verbs.add_safe_verb("assert_colormap", cog::value(), [](colormap_id) { });
            verbs.add_safe_verb("assert_keyframe", cog::value(), [](keyframe_id) { });
            verbs.add_safe_verb("assert_material", cog::value(), [](material_id) { });
            verbs.add_safe_verb("assert_model", cog::value(), [](model_id) { });
            verbs.add_safe_verb("assert_sound", cog::value(), [](sound_id) { });
            verbs.add_safe_verb("assert_template", cog::value(), [](thing_template_id) { });
            verbs.add_safe_verb("assert_message", cog::value(), [](message_id) { });
            verbs.add_safe_verb("assert_other", cog::value(), [](int) { });

            return;
        }
    };

}

MAKE_MAIN(gorc::cog_program)
