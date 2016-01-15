#pragma once

#include "io/path.hpp"
#include "program_type.hpp"
#include "external_lib_type.hpp"
#include <unordered_map>
#include <memory>
#include <set>
#include <map>

namespace gorc {

    class program_data {
    public:
        std::string name;
        path source_directory;
        std::vector<path> sources;
        std::vector<path> dependencies;
        program_type type = program_type::release;
        ext_lib_set external_libraries;
        bool exclude_test_coverage = false;

        program_data(path const &filename);
    };

    class library_data {
    public:
        std::string name;
        path source_directory;
        std::vector<path> sources;
        std::vector<path> dependencies;
        ext_lib_set external_libraries;
        bool exclude_test_coverage = false;

        library_data(path const &filename);
    };

    class project_file {
    public:
        std::set<path> project_files;

        std::map<path, std::unique_ptr<program_data>> programs;
        std::map<path, std::unique_ptr<library_data>> libraries;

        project_file(path const &filename);
    };

}
