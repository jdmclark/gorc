#include "gnu_compiler_properties.hpp"
#include "log/log.hpp"

gorc::gnu_compiler_properties::gnu_compiler_properties(path const &obj_dir,
                                                       path const &lib_dir,
                                                       path const &bin_dir,
                                                       path const &test_bin_dir,
                                                       path const &build_bin_dir)
    : obj_dir(obj_dir)
    , lib_dir(lib_dir)
    , bin_dir(bin_dir)
    , test_bin_dir(test_bin_dir)
    , build_bin_dir(build_bin_dir)
{
    return;
}

gorc::path gorc::gnu_compiler_properties::make_object_filename_from_psf(path const &psf)
{
    if(!psf.is_relative()) {
        LOG_FATAL(format("primary source file path '%s' must be relative to project root") %
                  psf.generic_string());
    }

    return (obj_dir / psf).replace_extension(".o");
}

gorc::path gorc::gnu_compiler_properties::make_library_filename(std::string const &name)
{
    return lib_dir / str(format("lib%s.a") % name);
}

gorc::path gorc::gnu_compiler_properties::make_program_filename(std::string const &name,
                                                                program_type type)
{
    path *root_path;
    switch(type) {
    default:
    case program_type::release:
        root_path = &bin_dir;
        break;

    case program_type::test:
        root_path = &test_bin_dir;
        break;

    case program_type::build:
        root_path = &build_bin_dir;
        break;
    }

    return (*root_path) / name;
}
