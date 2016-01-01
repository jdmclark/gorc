#pragma once

#include "io/path.hpp"
#include "program_type.hpp"
#include <vector>
#include <string>
#include <unordered_map>

namespace gorc {

    class compiler_configuration {
    public:
        path obj_path;
        path lib_path;
        path bin_path;
        path test_bin_path;
        path build_bin_path;
    };

    class object_file_entity;
    class library_file_entity;
    class program_file_entity;

    class compiler_properties {
    protected:
        compiler_configuration const &config;

    public:
        compiler_properties(compiler_configuration const &config);
        virtual ~compiler_properties();

        virtual path make_object_filename_from_psf(path const &psf) = 0;
        virtual path make_library_filename(std::string const &name) = 0;
        virtual path make_program_filename(std::string const &name,
                                           program_type type) = 0;

        virtual bool compile_object_file(object_file_entity *) = 0;
        virtual bool archive_static_library(library_file_entity *) = 0;
        virtual bool link_program(program_file_entity *) = 0;
    };

}
