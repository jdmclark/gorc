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
        "-std=c++14",
        "-DPLATFORM_LINUX",
        "-pthread",
        "-Werror",
        "-Wall",
        "-Wextra",
        "-Wpedantic",
        "-Wdouble-promotion",
        "-Wno-switch-enum",
        "-Wuninitialized",
        "-Wstrict-aliasing=3",
        "-Wstrict-overflow=1",
        "-Wfloat-equal",
        "-Wcast-qual",
        "-Wcast-align",
        "-Wconversion",
        "-Wzero-as-null-pointer-constant",
        "-Wlogical-op",
        "-Wdisabled-optimization",
        "-Wl,--no-as-needed"
    };

    std::vector<std::string> release_cflags {
        "-O3"
    };

    std::vector<std::string> debug_cflags {
        "-O0",
        "-ggdb"
    };

    std::vector<std::string> coverage_cflags {
        "-O0",
        "-g",
        "-fprofile-arcs",
        "-ftest-coverage",
        "-fPIC",
        "-fno-elide-constructors"
    };

    std::vector<std::string> lib_boost_cflags {
        "-lboost_system",
        "-lboost_filesystem"
    };

    std::vector<std::string> ld_postflags {
        "-ldl"
    };

    std::vector<std::string> const& get_type_cflags(gorc::build_type t)
    {
        switch(t) {
        default:
        case gorc::build_type::release:
            return release_cflags;

        case gorc::build_type::debug:
            return debug_cflags;

        case gorc::build_type::coverage:
            return coverage_cflags;
        }
    }
}

gorc::gnu_compiler_properties::gnu_compiler_properties(compiler_configuration const &config)
    : compiler_properties(config)
{
    for(auto const &search_path : config.header_search_paths) {
        header_search_cflags.push_back(str(format("-I%s") % search_path.native()));
    }
    return;
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
    LOG_DEBUG(format("Compiling %s") % entity->name());

    boost::filesystem::create_directories(entity->file_path().parent_path());

    std::vector<std::string> args;

    auto const &type_cflags = get_type_cflags(config.type);
    std::copy(type_cflags.begin(), type_cflags.end(), std::back_inserter(args));
    std::copy(common_cflags.begin(), common_cflags.end(), std::back_inserter(args));
    std::copy(header_search_cflags.begin(), header_search_cflags.end(), std::back_inserter(args));

    // Pass canonical paths to gcc so gcov can find source files
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
    LOG_DEBUG(format("Archiving %s") % entity->name());

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
    LOG_DEBUG(format("Linking %s") % prog->name());

    boost::filesystem::create_directories(prog->file_path().parent_path());

    // Need to order static libraries such that base precedes dependency
    std::vector<library_file_entity*> prog_libs;
    std::unordered_set<entity*> marked;
    for(library_file_entity *lib : prog->get_libraries()) {
        collect_ordered_libs(lib, prog_libs, marked);
    }

    std::vector<std::string> args;

    auto const &type_cflags = get_type_cflags(config.type);
    std::copy(type_cflags.begin(), type_cflags.end(), std::back_inserter(args));
    std::copy(common_cflags.begin(), common_cflags.end(), std::back_inserter(args));
    std::copy(header_search_cflags.begin(), header_search_cflags.end(), std::back_inserter(args));

    // Target name
    args.push_back("-o");
    args.push_back(prog->file_path().native());

    // Objects
    for(object_file_entity *obj : prog->get_objects()) {
        args.push_back(obj->file_path().native());
    }

    // Libraries
    args.push_back(str(format("-L%s") % config.lib_path.native()));

    ext_lib_set external_libs = prog->get_external_libraries();
    for(library_file_entity *lib : make_reverse_range(prog_libs)) {
        args.push_back(str(format("-l%s") % lib->get_library_name()));

        auto const &lib_extlibs = lib->get_external_libraries();
        std::copy(lib_extlibs.begin(), lib_extlibs.end(), std::inserter(external_libs,
                                                                        external_libs.begin()));
    }

    // Add external lib arguments in correct order
    if(external_libs.find(external_lib_type::boost) != external_libs.end()) {
        std::copy(lib_boost_cflags.begin(), lib_boost_cflags.end(), std::back_inserter(args));
    }

    // Add postflags
    std::copy(ld_postflags.begin(), ld_postflags.end(), std::back_inserter(args));

    process gcc("g++", // TODO: Compiler selection
                args,
                /* input */ nothing,
                /* output */ nothing,
                /* error */ nothing);

    return gcc.join() == 0;

    return true;
}
