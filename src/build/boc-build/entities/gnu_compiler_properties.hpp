#pragma once

#include "compiler_properties.hpp"

namespace gorc {

    class gnu_compiler_properties : public compiler_properties {
    private:
        path obj_dir;
        path lib_dir;
        path bin_dir;
        path test_bin_dir;
        path build_bin_dir;

    public:
        gnu_compiler_properties(path const &obj_dir,
                                path const &lib_dir,
                                path const &bin_dir,
                                path const &test_bin_dir,
                                path const &build_bin_dir);

        virtual path make_object_filename_from_psf(path const &psf) override;
        virtual path make_library_filename(std::string const &name) override;
        virtual path make_program_filename(std::string const &name,
                                           program_type type) override;
    };

}
