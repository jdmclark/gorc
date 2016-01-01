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
    };

}
