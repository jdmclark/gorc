#include "program/program.hpp"
#include "io/path.hpp"
#include "register_entities.hpp"
#include "project_graph.hpp"
#include "list_targets.hpp"
#include "run_build.hpp"
#include "utility/service_registry.hpp"
#include "entities/gnu_compiler_properties.hpp"
#include "build/common/change_to_project_root.hpp"
#include "build/common/paths.hpp"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

namespace {

    std::unordered_map<std::string, gorc::build_type> build_type_map {
        { "release", gorc::build_type::release },
        { "debug", gorc::build_type::debug },
        { "coverage", gorc::build_type::coverage }
    };

}

namespace gorc {

    class boc_build_program : public program {
    public:
        bool do_nothing = false;
        bool list_targets_only = false;

        path original_working_directory;
        path project_root_path;
        path original_working_directory_rel;

        std::string custom_build_type;

        build_type get_build_type() const
        {
            if(custom_build_type.empty()) {
                return build_type::release;
            }

            auto it = build_type_map.find(custom_build_type);
            if(it == build_type_map.end()) {
                LOG_FATAL(format("Unknown build type '%s'") % custom_build_type);
            }

            return it->second;
        }

        virtual void create_options(options &opts) override
        {
            // Commands
            opts.insert(make_switch_option("do-nothing", do_nothing));
            opts.insert(make_switch_option("list-targets", list_targets_only));

            opts.emplace_constraint<mutual_exclusion>(
                    std::vector<std::string> { "do-nothing",
                                               "list-targets" });

            // Build options
            opts.insert(make_value_option("type", custom_build_type));

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
            else {
                return run_build(services, pg.get_root());
            }
        }
    };

}

MAKE_MAIN(gorc::boc_build_program)
