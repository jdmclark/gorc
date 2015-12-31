#pragma once

#include "io/path.hpp"
#include "program_type.hpp"

namespace gorc {

    class compiler_properties {
    public:
        virtual ~compiler_properties();

        virtual path make_object_filename_from_psf(path const &psf) = 0;
        virtual path make_library_filename(std::string const &name) = 0;
        virtual path make_program_filename(std::string const &name,
                                           program_type type) = 0;
    };

}
