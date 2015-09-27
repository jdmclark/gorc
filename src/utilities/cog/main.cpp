#include "program/program.hpp"
#include "cog/compiler/compiler.hpp"
#include "io/native_file.hpp"
#include "cog/vm/vm.hpp"
#include "cog/vm/instance.hpp"
#include <vector>

namespace gorc {

    class cog_program : public program {
    private:
        std::vector<std::string> cog_files;
        cog::verb_table verbs;

    public:
        virtual void create_options(options &opts) override
        {
            opts.insert_bare(make_bare_multi_value_option(std::back_inserter(cog_files)));

            opts.emplace_constraint<at_least_one_input>();
        }

        virtual int main() override
        {
            populate_verb_table();

            cog::constant_table constants;
            cog::default_populate_constant_table(constants);

            cog::compiler compiler(verbs, constants);

            std::vector<std::unique_ptr<cog::script>> scripts;

            for(auto const &cog_file : cog_files) {
                diagnostic_context dc(cog_file.c_str());
                try {
                    auto f = make_native_read_only_file(cog_file);
                    scripts.push_back(compiler.compile(*f));
                }
                catch(logged_runtime_error const &) {
                    return EXIT_FAILURE;
                }
                catch(std::exception const &e) {
                    LOG_ERROR(e.what());
                    return EXIT_FAILURE;
                }
            }

            // Construct instances:
            std::vector<std::unique_ptr<cog::instance>> instances;
            for(auto const &script : scripts) {
                instances.push_back(make_unique<cog::instance>(*script));
            }

            // Execute startup messages:
            for(auto &instance : instances) {
                auto msg_addr = instance->cog.exports.get_offset(cog::message_type::startup);
                if(!msg_addr.has_value()) {
                    continue;
                }

                cog::vm vm;
                vm.execute(verbs, instance->cog, instance->memory, msg_addr.get_value());
            }

            return EXIT_SUCCESS;
        }

        void populate_verb_table()
        {
            verbs.add_verb("printint", [](int v) {
                    std::cout << v << std::endl;
                });

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
