#include "program/program.hpp"
#include "io/path.hpp"
#include "register_entities.hpp"
#include "project_graph.hpp"
#include "list_targets.hpp"
#include "run_build.hpp"
#include "print_status.hpp"
#include "utility/shell_progress.hpp"
#include "utility/file_progress.hpp"
#include "utility/service_registry.hpp"
#include "entities/gnu_compiler_properties.hpp"
#include "build/common/change_to_project_root.hpp"
#include "build/common/paths.hpp"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

namespace gorc {

    class boc_build_program : public program {
    public:
        bool do_nothing = false;
        bool list_targets_only = false;
        bool print_build_summary = false;
        bool print_status = false;
        bool no_progress = false;
        size_t threads = 1;

        path original_working_directory;
        path project_root_path;
        path original_working_directory_rel;

        std::string custom_build_type;

        build_type get_build_type() const
        {
            if(custom_build_type.empty()) {
                return build_type::release;
            }

            return to_build_type(custom_build_type);
        }

        virtual void create_options(options &opts) override
        {
            // Commands
            opts.insert(make_switch_option("do-nothing", do_nothing));
            opts.insert(make_switch_option("list-targets", list_targets_only));
            opts.insert(make_switch_option("status", print_status));

            opts.emplace_constraint<mutual_exclusion>(
                    std::vector<std::string> { "do-nothing",
                                               "list-targets",
                                               "status" });

            // Build options
            opts.insert(make_value_option("type", custom_build_type));
            opts.insert(make_switch_option("print-summary", print_build_summary));
            opts.insert(make_switch_option("no-progress", no_progress));

            opts.insert(make_value_option("threads", threads));
            opts.add_alias("threads", "-j");

            return;
        }

        virtual int main() override
        {
            change_to_project_root(original_working_directory,
                                   project_root_path,
                                   original_working_directory_rel);

            // Project root was located. Set up engine.
            service_registry services;

            // TODO: Compiler selection
            compiler_configuration comp_config;
            comp_config.obj_path = "pkg/obj";
            comp_config.lib_path = "pkg/lib";
            comp_config.bin_path = "pkg/bin";
            comp_config.test_bin_path = "pkg/test-bin";
            comp_config.build_bin_path = "pkg/build-bin";
            comp_config.type = get_build_type();
            comp_config.header_search_paths = {
                "src",
                "src/libs"
            };

            gnu_compiler_properties comp_props(comp_config);
            services.add<compiler_properties>(comp_props);

            std::unique_ptr<progress_factory> prog_fac;
            if(no_progress) {
                prog_fac = std::make_unique<file_progress_factory>();
            }
            else {
                prog_fac = std::make_unique<shell_progress_factory>();
            }

            services.add<progress_factory>(*prog_fac);

            // Build is allowed to create new entities to track real (file) dependencies.
            // Add entity allocator as a service to store these until project_graph destructor.
            entity_allocator ea;
            services.add<entity_allocator>(ea);

            entity_registry reg;
            register_boc_entities(reg);

            // Initialize graph
            project_graph pg(services, reg, ea, boc_project_filename, boc_cache_filename);

            // Perform operation
            if(do_nothing) {
                return EXIT_SUCCESS;
            }
            else if(list_targets_only) {
                list_targets(pg.get_root());
                return EXIT_SUCCESS;
            }
            else if(print_status) {
                gorc::print_status(pg.get_root());
                return EXIT_SUCCESS;
            }
            else {
                return run_build(services, pg.get_root(), threads, print_build_summary);
            }
        }
    };

}

MAKE_MAIN(gorc::boc_build_program)
