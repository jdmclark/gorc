#include "gnu_compiler_properties.hpp"
#include "log/log.hpp"
#include "system/process.hpp"
#include "object_file_entity.hpp"
#include "library_file_entity.hpp"
#include "program_file_entity.hpp"
#include "utility/range.hpp"
#include <unordered_set>
#include <boost/filesystem.hpp>

namespace {

    std::vector<std::string> common_cflags {
        "-std=c++11",
        "-DPLATFORM_LINUX",
        "-Isrc",
        "-Isrc/libs",
        "-pthread",
        "-Werror",
        "-Wall",
        "-Wextra",
        "-Wpedantic",
        "-Wdouble-promotion",
        "-Wno-switch-enum",
        "-Wswitch",
        "-Wuninitialized",
        "-Wstrict-aliasing=3",
        "-Wstrict-overflow=1",
        "-Wtrampolines",
        "-Wfloat-equal",
        "-Wcast-qual",
        "-Wcast-align",
        "-Wwrite-strings",
        "-Wconversion",
        "-Wlogical-op",
        "-Wno-aggressive-loop-optimizations",
        "-Wvector-operation-performance",
        "-Wvla",
        "-Wdisabled-optimization",
        "-Wno-unused-variable",
        "-Wno-unused-local-typedefs",
        "-Wno-format",
        "-Wl,--no-as-needed"
    };

    std::vector<std::string> release_cflags {
        "-O3"
    };
}

gorc::path gorc::gnu_compiler_properties::make_object_filename_from_psf(path const &psf)
{
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

bool gorc::gnu_compiler_properties::compile_object_file(object_file_entity *entity)
{
    LOG_INFO(format("Compiling %s") % entity->name());

    boost::filesystem::create_directories(entity->file_path().parent_path());

    std::vector<std::string> args;

    std::copy(release_cflags.begin(), release_cflags.end(), std::back_inserter(args));
    std::copy(common_cflags.begin(), common_cflags.end(), std::back_inserter(args));

    args.push_back("-c");
    args.push_back(entity->primary_source_file->file_path().native());
    args.push_back("-o");
    args.push_back(entity->file_path().native());

    process gcc("g++", // TODO: Compiler selection
                args,
                /* input */ nothing,
                /* output */ nothing,
                /* error */ nothing);

    return gcc.join() == 0;
}

bool gorc::gnu_compiler_properties::archive_static_library(library_file_entity *entity)
{
    LOG_INFO(format("Archiving %s") % entity->name());

    boost::filesystem::create_directories(entity->file_path().parent_path());

    std::vector<std::string> args;
    args.push_back("rcsT");

    // Target name
    args.push_back(entity->file_path().native());

    // Objects
    for(object_file_entity *obj : entity->get_objects()) {
        args.push_back(obj->file_path().native());
    }

    process ar("ar", // TODO: Compiler selection
               args,
               /* input */ nothing,
               /* output */ nothing,
               /* error */ nothing);

    return ar.join() == 0;
}

namespace {

    void collect_ordered_libs(gorc::library_file_entity *lib,
                              std::vector<gorc::library_file_entity*> &out_libs,
                              std::unordered_set<gorc::entity*> &marked)
    {
        if(marked.find(lib) != marked.end()) {
            return;
        }

        marked.insert(lib);

        for(gorc::library_file_entity *lib_dep : lib->get_libraries()) {
            collect_ordered_libs(lib_dep, out_libs, marked);
        }

        out_libs.push_back(lib);
    }

}

bool gorc::gnu_compiler_properties::link_program(program_file_entity *prog)
{
    LOG_INFO(format("Linking %s") % prog->name());

    boost::filesystem::create_directories(prog->file_path().parent_path());

    // Need to order static libraries such that base precedes dependency
    std::vector<library_file_entity*> prog_libs;
    std::unordered_set<entity*> marked;
    for(library_file_entity *lib : prog->get_libraries()) {
        collect_ordered_libs(lib, prog_libs, marked);
    }

    std::vector<std::string> args;

    std::copy(release_cflags.begin(), release_cflags.end(), std::back_inserter(args));
    std::copy(common_cflags.begin(), common_cflags.end(), std::back_inserter(args));

    // Target name
    args.push_back("-o");
    args.push_back(prog->file_path().native());

    // Objects
    for(object_file_entity *obj : prog->get_objects()) {
        args.push_back(obj->file_path().native());
    }

    // Libraries
    args.push_back(str(format("-L%s") % config.lib_path.native()));

    for(library_file_entity *lib : make_reverse_range(prog_libs)) {
        args.push_back(str(format("-l%s") % lib->get_library_name()));
    }

    process gcc("g++", // TODO: Compiler selection
                args,
                /* input */ nothing,
                /* output */ nothing,
                /* error */ nothing);

    return gcc.join() == 0;

    return true;
}
