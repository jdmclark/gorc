#pragma once

#include "compiler_properties.hpp"

namespace gorc {

    class gnu_compiler_properties : public compiler_properties {
    public:
        using compiler_properties::compiler_properties;

        virtual path make_object_filename_from_psf(path const &psf) override;
        virtual path make_library_filename(std::string const &name) override;
        virtual path make_program_filename(std::string const &name,
                                           program_type type) override;
    };

}
