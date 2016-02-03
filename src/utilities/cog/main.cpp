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

    class cog_program : public program {
    private:
        std::string scenario_file;
        cog::verb_table verbs;

    public:
        virtual void create_options(options &opts) override
        {
            opts.insert(make_value_option("scenario", scenario_file));

            opts.emplace_constraint<required_option>("scenario");
        }

        virtual int main() override
        {
            cog::default_populate_verb_table(verbs);
            populate_verb_table();

            cog::constant_table constants;
            cog::default_populate_constant_table(constants);

            service_registry services;

            loader_registry loaders;
            loaders.emplace_loader<cog::script_loader>();
            services.add(loaders);

            cog::compiler compiler(verbs, constants);
            services.add(compiler);

            native_file_system vfs;
            services.add<virtual_file_system>(vfs);

            content_manager content(services);
            services.add(content);

            // Load scenario file
            diagnostic_context dc(scenario_file.c_str());
            auto f = make_native_read_only_file(scenario_file);
            json_input_stream jis(*f);
            cog_scenario scenario(deserialization_constructor, jis);

            // Construct instances:
            cog::executor executor(verbs);
            for(auto const &file : scenario.cog_files) {
                auto script = content.load<cog::script>(file.cog_filename);
                cog::instance *instance;
                if(file.init.empty()) {
                    instance = &executor.create_instance(script);
                }
                else {
                    instance = &executor.create_instance(script, file.init);
                }

                // Fake loading phase: loop over resource symbols and rebind
                for(auto const &sym : script->symbols) {
                    if(is_resource_id_type(sym.type)) {
                        auto &cel = instance->memory[sym.sequence_number];

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

            // Execute startup messages:
            executor.send_to_all(cog::message_type::startup,
                                 /* sender: nothing */ 0,
                                 /* source: nothing */ 0,
                                 /* param0 */ cog::value(),
                                 /* param1 */ cog::value(),
                                 /* param2 */ cog::value(),
                                 /* param3 */ cog::value());

            time_delta current_time = 0.0s;
            while(current_time < scenario.max_time) {
                current_time += scenario.time_step;
                std::cout << "T+" << current_time.count() << std::endl;

                executor.update(scenario.time_step);
            }

            return EXIT_SUCCESS;
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
                    std::cout << cog::as_string(stk.top()) << std::endl;
                    stk.pop();
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

            return;
        }
    };

}

MAKE_MAIN(gorc::cog_program)
