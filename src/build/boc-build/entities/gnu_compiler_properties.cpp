#include "gnu_compiler_properties.hpp"
#include "log/log.hpp"

gorc::path gorc::gnu_compiler_properties::make_object_filename_from_psf(path const &psf)
{
    if(!psf.is_relative()) {
        LOG_FATAL(format("primary source file path '%s' must be relative to project root") %
                  psf.generic_string());
    }

    return (config.obj_path / psf).replace_extension(".o");
}

gorc::path gorc::gnu_compiler_properties::make_library_filename(std::string const &name)
{
    return config.lib_path / str(format("lib%s.a") % name);
}

gorc::path gorc::gnu_compiler_properties::make_program_filename(std::string const &name,
                                                                program_type type)
{
    path const *root_path;
    switch(type) {
    default:
    case program_type::release:
        root_path = &config.bin_path;
        break;

    case program_type::test:
        root_path = &config.test_bin_path;
        break;

    case program_type::build:
        root_path = &config.build_bin_path;
        break;
    }

    return (*root_path) / name;
}
