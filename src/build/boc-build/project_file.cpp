#include "project_file.hpp"
#include "io/native_file.hpp"
#include "log/log.hpp"
#include "text/json_input_stream.hpp"
#include "text/json_specification.hpp"
#include "utility/make_unique.hpp"
#include "paths.hpp"
#include <set>
#include <map>

using namespace gorc;

namespace {

    class root_project_data {
    public:
        std::vector<path> programs;

        root_project_data(path const &filename);
    };

    json_specification<root_project_data> root_project_data_spec(
            /* Members */
            {
                { "programs", [](json_input_stream &jis, root_project_data &obj) {
                    json_deserialize_array<std::string>(jis, std::back_inserter(obj.programs)); } }
            });

    root_project_data::root_project_data(path const &filename)
    {
        auto str_filename = filename.generic_string();
        diagnostic_context dc(str_filename.c_str());

        std::unique_ptr<input_stream> in_file;
        try {
            in_file = make_native_read_only_file(filename);
        }
        catch(std::exception const &e) {
            LOG_FATAL(e.what());
        }

        json_input_stream jis(*in_file);
        json_deserialize_with_specification(jis, root_project_data_spec, *this);
    }

    std::unordered_map<std::string, program_type> program_type_map {
        { "release", program_type::release },
        { "test", program_type::test },
        { "build", program_type::build }
    };

    json_specification<program_data> program_data_spec(
            /* Members */
            {
                { "name", make_json_member(&program_data::name) },
                { "sources", [](json_input_stream &jis, program_data &obj) {
                    json_deserialize_array<std::string>(jis, std::back_inserter(obj.sources)); } },
                { "dependencies", [](json_input_stream &jis, program_data &obj) {
                    json_deserialize_array<std::string>(jis,
                                                        std::back_inserter(obj.dependencies)); } },
                { "type", [](json_input_stream &jis, program_data &obj) {
                    std::string type = json_deserialize<std::string>(jis);
                    auto it = program_type_map.find(type);
                    if(it == program_type_map.end()) {
                        LOG_FATAL(format("unknown program type '%s'") % type);
                    }

                    obj.type = it->second;
                } }
            },

            /* Required */
            { "name" });

    json_specification<library_data> library_data_spec(
            /* Members */
            {
                { "name", make_json_member(&library_data::name) },
                { "sources", [](json_input_stream &jis, library_data &obj) {
                    json_deserialize_array<std::string>(jis, std::back_inserter(obj.sources)); } },
                { "dependencies", [](json_input_stream &jis, library_data &obj) {
                    json_deserialize_array<std::string>(jis,
                                                        std::back_inserter(obj.dependencies)); } }
            },

            /* Required */
            { "name" });

}

program_data::program_data(path const &filename)
{
    source_directory = filename.parent_path();

    auto str_filename = filename.generic_string();
    diagnostic_context dc(str_filename.c_str());

    std::unique_ptr<input_stream> in_file;
    try {
        in_file = make_native_read_only_file(filename);
    }
    catch(std::exception const &e) {
        LOG_FATAL(e.what());
    }

    json_input_stream jis(*in_file);
    json_deserialize_with_specification(jis, program_data_spec, *this);
}

library_data::library_data(path const &filename)
{
    source_directory = filename.parent_path();

    auto str_filename = filename.generic_string();
    diagnostic_context dc(str_filename.c_str());

    std::unique_ptr<input_stream> in_file;
    try {
        in_file = make_native_read_only_file(filename);
    }
    catch(std::exception const &e) {
        LOG_FATAL(e.what());
    }

    json_input_stream jis(*in_file);
    json_deserialize_with_specification(jis, library_data_spec, *this);
}

gorc::project_file::project_file(path const &project_filename)
{
    project_files.insert(project_filename);
    std::string str_filename = project_filename.generic_string();
    diagnostic_context dc(str_filename.c_str());

    // Load basic project data from root JSON file
    root_project_data root_data(project_filename);

    std::set<path> unmet_dependencies;

    // Load programs
    for(auto const &prog : root_data.programs) {
        auto prog_path = boc_src_directory / prog / boc_prog_filename;
        project_files.insert(prog_path);

        auto res = programs.emplace(prog, make_unique<program_data>(prog_path));

        if(!res.second) {
            LOG_FATAL(format("program '%s' is included multiple times") % prog.generic_string());
        }

        for(auto const &dep : res.first->second->dependencies) {
            unmet_dependencies.insert(dep);
        }
    }

    // Load libraries
    while(!unmet_dependencies.empty()) {
        auto libit = unmet_dependencies.begin();
        path libpath = *libit;
        unmet_dependencies.erase(libit);

        auto combined_lib_path = boc_src_directory / libpath / boc_lib_filename;
        project_files.insert(combined_lib_path);

        auto res = libraries.emplace(libpath, make_unique<library_data>(combined_lib_path));

        for(auto const &dep : res.first->second->dependencies) {
            auto loaded_it = libraries.find(dep);
            if(loaded_it == libraries.end()) {
                unmet_dependencies.insert(dep);
            }
        }
    }
}
