#pragma once

#include "compiler_properties.hpp"
#include <vector>
#include <string>

namespace gorc {

    class gnu_compiler_properties : public compiler_properties {
    private:
        std::vector<std::string> header_search_cflags;

    public:
        gnu_compiler_properties(compiler_configuration const &config);

        virtual path make_object_filename_from_psf(path const &psf) override;
        virtual path make_library_filename(std::string const &name) override;
        virtual path make_program_filename(std::string const &name,
                                           program_type type) override;

        virtual bool compile_object_file(object_file_entity *) override;
        virtual bool archive_static_library(library_file_entity *) override;
        virtual bool link_program(program_file_entity *) override;
    };

}
