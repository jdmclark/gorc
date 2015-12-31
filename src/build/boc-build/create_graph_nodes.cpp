#include "create_graph_nodes.hpp"

#include "entities/file_entity.hpp"
#include "entities/source_file_entity.hpp"
#include "entities/graph_entity.hpp"
#include "entities/library_file_entity.hpp"
#include "entities/source_file_entity.hpp"
#include "entities/object_file_entity.hpp"
#include "entities/program_file_entity.hpp"
#include "entities/project_entity.hpp"

#include <boost/filesystem.hpp>

using namespace gorc;

namespace {

    object_file_entity* get_or_create_object(path const &psf_name,
                                             service_registry const &services,
                                             entity_allocator &ea,
                                             std::map<path, object_file_entity*> objects)
    {
        auto existing_it = objects.find(psf_name);
        if(existing_it != objects.end()) {
            return existing_it->second;
        }

        source_file_entity *psf_ent = ea.emplace<source_file_entity>(psf_name);
        object_file_entity *obj_ent = ea.emplace<object_file_entity>(psf_ent, services);

        objects.emplace(psf_name, obj_ent);

        return obj_ent;
    }

}

root_entity* gorc::create_graph_nodes(project_file const &pf,
                                      service_registry const &services,
                                      entity_allocator &ea)
{
    // Create graph nodes
    std::unordered_set<entity*> project_files;
    for(auto const &subfile : pf.project_files) {
        project_files.insert(ea.emplace<file_entity>(
                    subfile,
                    boost::filesystem::last_write_time(subfile)));
    }

    graph_entity *graph_ent = ea.emplace<graph_entity>(project_files);

    // Create project nodes
    std::map<path, object_file_entity*> objects;
    std::map<path, library_file_entity*> libraries;

    for(auto const &library : pf.libraries) {
        std::unordered_set<object_file_entity*> library_objects;
        for(auto const &src : library.second->sources) {
            library_objects.insert(get_or_create_object(src, services, ea, objects));
        }

        library_file_entity *libent = ea.emplace<library_file_entity>(library.second->name,
                                                                      library_objects,
                                                                      services);

        libraries.emplace(library.first, libent);
    }

    std::unordered_set<program_file_entity*> programs;

    for(auto const &prog : pf.programs) {
        std::unordered_set<object_file_entity*> program_objects;
        for(auto const &src : prog.second->sources) {
            program_objects.insert(get_or_create_object(src, services, ea, objects));
        }

        // Compute unique closure of static library dependencies
        std::set<path> open_libs;
        std::copy(prog.second->dependencies.begin(),
                  prog.second->dependencies.end(),
                  std::inserter(open_libs, open_libs.begin()));

        std::set<path> closed_libs;
        std::unordered_set<library_file_entity*> program_libraries;

        while(!open_libs.empty()) {
            auto it = open_libs.begin();
            path opened_lib = *it;
            open_libs.erase(it);

            closed_libs.insert(opened_lib);
            program_libraries.insert(libraries.at(opened_lib));

            for(auto const &next_lib : pf.libraries.at(opened_lib)->dependencies) {
                if(closed_libs.find(next_lib) != closed_libs.end()) {
                    // Library has not been visited yet
                    open_libs.insert(next_lib);
                }
            }
        }

        program_file_entity *prog_ent = ea.emplace<program_file_entity>(prog.second->name,
                                                                        program_type::release, // TODO
                                                                        program_objects,
                                                                        program_libraries,
                                                                        services);
        programs.insert(prog_ent);
    }

    project_entity *proj_ent = ea.emplace<project_entity>(programs);

    return ea.emplace<root_entity>(graph_ent, proj_ent);
}
