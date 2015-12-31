#include "program/program.hpp"
#include "io/path.hpp"
#include "register_entities.hpp"
#include "project_graph.hpp"
#include "list_targets.hpp"
#include "utility/service_registry.hpp"
#include "entities/gnu_compiler_properties.hpp"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

namespace gorc {

    class boc_build_program : public program {
    public:
        std::string boc_root_filename = "project.json";
        std::string boc_root_filename_override;

        std::string boc_cache_filename = ".boc-cache";
        std::string boc_cache_filename_override;

        bool do_nothing = false;
        bool list_targets_only = false;

        path original_working_directory;
        path project_root_path;

        virtual void create_options(options &opts) override
        {
            // Test and debugging options
            opts.insert(make_value_option("override-root-name", boc_root_filename_override));
            opts.insert(make_value_option("override-cache-name", boc_cache_filename_override));

            opts.insert(make_switch_option("do-nothing", do_nothing));
            opts.insert(make_switch_option("list-targets", list_targets_only));

            opts.emplace_constraint<mutual_exclusion>(
                    std::vector<std::string> { "do-nothing",
                                               "list-targets" });

            return;
        }

        void change_to_project_root()
        {
            while(true) {
                if(is_regular_file(boc_root_filename)) {
                    project_root_path = canonical(current_path());
                    return;
                }

                auto canonical_current_path = canonical(current_path());

                if(!canonical_current_path.has_parent_path()) {
                    LOG_FATAL("current directory is not a boc project");
                }

                current_path(canonical_current_path.parent_path());
            }
        }

        virtual int main() override
        {
            if(!boc_root_filename_override.empty()) {
                boc_root_filename = boc_root_filename_override;
            }

            if(!boc_cache_filename_override.empty()) {
                boc_cache_filename = boc_cache_filename_override;
            }

            original_working_directory = canonical(current_path());
            change_to_project_root();

            // Project root was located. Set up engine.
            service_registry services;

            // TODO: Compiler selection
            gnu_compiler_properties comp_props(
                    "pkg/obj" /* obj path */,
                    "pkg/lib" /* lib path */,
                    "pkg/bin" /* release bin path */,
                    "pkg/test-bin" /* test bin path */,
                    "pkg/build-bin" /* build bin path */);
            services.add<compiler_properties>(comp_props);

            entity_registry reg;
            register_boc_entities(reg);

            // Initialize graph
            project_graph pg(services, reg, boc_root_filename, boc_cache_filename);

            // Perform operation
            if(do_nothing) {
                return EXIT_SUCCESS;
            }
            else if(list_targets_only) {
                list_targets(pg.get_root());
                return EXIT_SUCCESS;
            }

            return EXIT_SUCCESS;
        }
    };

}

MAKE_MAIN(gorc::boc_build_program)
