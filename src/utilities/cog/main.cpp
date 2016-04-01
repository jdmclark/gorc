#include "program/program.hpp"
#include "cog/compiler/compiler.hpp"
#include "cog/compiler/script_loader.hpp"
#include "io/native_file.hpp"
#include "cog/vm/executor.hpp"
#include "cog/vm/default_verbs.hpp"
#include "utility/zip.hpp"
#include "utility/time.hpp"
#include "scenario.hpp"
#include "vfs/native_file_system.hpp"
#include "content/content_manager.hpp"
#include "content/loader_registry.hpp"
#include <vector>
#include <unordered_map>
#include <iostream>

namespace gorc {

    class cog_program : public program, public cog_scenario_event_visitor {
    private:
        std::string scenario_file;
        cog::verb_table verbs;

        std::string save_file;
        bool load_from_save = false;

        bool save_and_exit_when_possible = false;

        std::unique_ptr<content_manager> content;
        std::unique_ptr<cog::executor> executor;
        time_delta current_time = 0.0s;

        void serialize_to_savefile(service_registry const &services)
        {
            auto out_stream = make_native_file(save_file);
            binary_output_stream bos(*out_stream, services);

            // Always serialize content manager first
            binary_serialize(bos, *content);
            binary_serialize(bos, *executor);
            binary_serialize(bos, current_time);
        }

        void instantiate_from_savefile(service_registry &services)
        {
            auto in_stream = make_native_read_only_file(save_file);
            binary_input_stream bis(*in_stream, services);

            content = std::make_unique<content_manager>(deserialization_constructor, bis);
            services.add(*content);

            executor = std::make_unique<cog::executor>(deserialization_constructor, bis);
            current_time = binary_deserialize<time_delta>(bis);
        }

        void instantiate_new(cog_scenario &scenario,
                             cog::verb_table &verbs,
                             service_registry &services)
        {
            content = std::make_unique<content_manager>(services);
            services.add(*content);

            executor = std::make_unique<cog::executor>(verbs);
            for(auto const &file : scenario.cog_files) {
                auto script = content->load<cog::script>(file.cog_filename);
                cog_id instance_id;
                if(file.init.empty()) {
                    instance_id = executor->create_instance(script);
                }
                else {
                    instance_id = executor->create_instance(script, file.init);
                }

                cog::instance &instance = executor->get_instance(instance_id);

                // Fake loading phase: loop over resource symbols and rebind
                for(auto const &sym : script->symbols) {
                    if(is_resource_id_type(sym.type)) {
                        auto &cel = instance.memory[sym.sequence_number];

                        // Skip unless it's a likely filename
                        if(cel.get_type() != cog::value_type::string) {
                            continue;
                        }

                        std::string name = static_cast<char const *>(cel);
                        auto it = scenario.resources.find(name);
                        if(it == scenario.resources.end()) {
                            LOG_FATAL(format("no scenario resource named '%s'") % name);
                        }

                        cel = it->second;
                    }
                }
            }

            current_time = 0.0s;

            // Execute startup messages:
            executor->send_to_all(cog::message_type::startup,
                                  /* sender: nothing */ 0,
                                  /* senderid: nothing */ cog::value(),
                                  /* source: nothing */ 0,
                                  /* param0 */ cog::value(),
                                  /* param1 */ cog::value(),
                                  /* param2 */ cog::value(),
                                  /* param3 */ cog::value());
        }

    public:
        virtual void create_options(options &opts) override
        {
            opts.insert(make_value_option("scenario", scenario_file));

            opts.insert(make_value_option("save-file", save_file));
            opts.insert(make_switch_option("load-from-save", load_from_save));

            opts.emplace_constraint<required_option>("scenario");
            opts.emplace_constraint<dependent_option>("load-from-save", "save-file");
        }

        virtual int run() override
        {
            cog::constant_table constants;
            cog::default_populate_constant_table(constants);

            service_registry services;

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

            // Construct instances:
            if(load_from_save) {
                LOG_INFO("Loading state");
                instantiate_from_savefile(services);
            }
            else {
                instantiate_new(scenario, verbs, services);
            }

            for(auto const &event : scenario.events) {
                if(save_and_exit_when_possible) {
                    serialize_to_savefile(services);
                    LOG_INFO("Saved state");
                    return EXIT_SUCCESS;
                }

                event->accept(*this);
            }

            return EXIT_SUCCESS;
        }

        virtual void visit(time_step_event const &e) override
        {
            current_time += e.step;
            std::cout << "T+" << current_time.count() << std::endl;
            executor->update(e.step);
        }

        virtual void visit(send_linked_event const &e) override
        {
            executor->send_to_linked(e.msg,
                                     e.sender,
                                     e.source,
                                     e.st,
                                     e.param0,
                                     e.param1,
                                     e.param2,
                                     e.param3);
        }

        void populate_verb_table()
        {
            verbs.add_verb("saveandquit", [&]() {
                    save_and_exit_when_possible = true;
                });

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

            verbs.add_verb("bitclear", [](int flag, int clearflags) {
                    return flag & (~clearflags);
                });

            verbs.add_verb("bitset", [](int flag, int setflags) {
                    return flag | setflags;
                });

            verbs.add_verb("bittest", [](int flag, int setflags) {
                    return flag & setflags;
                });

            verbs.add_verb("getglobalcog", [&](char const *cogname) {
                    return executor->create_global_instance(content->load<cog::script>(cogname));
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
