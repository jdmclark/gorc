#include "create_graph_nodes.hpp"

#include "entities/file_entity.hpp"
#include "entities/source_file_entity.hpp"
#include "entities/graph_entity.hpp"
#include "entities/library_file_entity.hpp"
#include "entities/source_file_entity.hpp"
#include "entities/object_file_entity.hpp"
#include "entities/program_file_entity.hpp"
#include "entities/project_entity.hpp"
#include "utility/join.hpp"

#include <vector>
#include <boost/filesystem.hpp>

using namespace gorc;

namespace {

    object_file_entity* get_or_create_object(path const &psf_name,
                                             service_registry const &services,
                                             entity_allocator &ea,
                                             std::map<path, object_file_entity*> &objects)
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

    library_file_entity* get_or_create_library(path const &lib_name,
                                               library_data const &libdata,
                                               service_registry const &services,
                                               entity_allocator &ea,
                                               std::map<path, object_file_entity*> &objects,
                                               std::map<path, library_file_entity*> &libraries,
                                               project_file const &pf,
                                               std::vector<path> &library_stack)
    {
        library_stack.push_back(lib_name);
        auto existing_it = libraries.find(lib_name);
        if(existing_it != libraries.end()) {
            if(existing_it->second == nullptr) {
                LOG_FATAL(format("cannot generate dependency graph: "
                                 "cycle detected in libraries\n%s") %
                          join(library_stack, " =>\n"));
            }

            library_stack.pop_back();
            return existing_it->second;
        }

        // Add nullptr to indicate that processing has started
        libraries.emplace(lib_name, nullptr);

        std::unordered_set<object_file_entity*> library_objects;
        for(auto const &src : libdata.sources) {
            path assembled_src = (libdata.source_directory / src).normalize();
            library_objects.insert(get_or_create_object(assembled_src, services, ea, objects));
        }

        std::unordered_set<library_file_entity*> library_deps;
        for(auto const &lib : libdata.dependencies) {
            library_deps.insert(get_or_create_library(lib,
                                                      *pf.libraries.at(lib),
                                                      services,
                                                      ea,
                                                      objects,
                                                      libraries,
                                                      pf,
                                                      library_stack));
        }

        library_file_entity *libent = ea.emplace<library_file_entity>(libdata.name,
                                                                      library_objects,
                                                                      library_deps,
                                                                      services);

        // Replace nullptr with assembled library
        libraries[lib_name] = libent;

        library_stack.pop_back();
        return libent;
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

    std::vector<path> library_stack;

    for(auto const &library : pf.libraries) {
        libraries.emplace(library.first, get_or_create_library(library.first,
                                                               *library.second,
                                                               services,
                                                               ea,
                                                               objects,
                                                               libraries,
                                                               pf,
                                                               library_stack));
    }

    std::unordered_set<program_file_entity*> programs;

    for(auto const &prog : pf.programs) {
        std::unordered_set<object_file_entity*> program_objects;
        for(auto const &src : prog.second->sources) {
            path assembled_src = (prog.second->source_directory / src).normalize();
            program_objects.insert(get_or_create_object(assembled_src, services, ea, objects));
        }

        std::unordered_set<library_file_entity*> program_libraries;
        for(auto const &lib : prog.second->dependencies) {
            program_libraries.insert(libraries.at(lib));
        }

        program_file_entity *prog_ent = ea.emplace<program_file_entity>(prog.second->name,
                                                                        prog.second->type,
                                                                        program_objects,
                                                                        program_libraries,
                                                                        services);
        programs.insert(prog_ent);
    }

    project_entity *proj_ent = ea.emplace<project_entity>(programs);

    return ea.emplace<root_entity>(graph_ent, proj_ent);
}
