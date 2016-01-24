#include "library_file_entity.hpp"
#include "build/boc-build/engine/entity_serializer.hpp"
#include "build/boc-build/engine/entity_deserializer.hpp"
#include "compiler_properties.hpp"
#include <boost/filesystem.hpp>

gorc::library_file_entity_internal_properties::library_file_entity_internal_properties(
        entity_input_stream &is)
    : library_name(is.read_string())
    , external_libs(is.read_enum_set<ext_lib_set>())
    , objects(is.read_entity_set<object_file_entity>())
    , libraries(is.read_entity_set<library_file_entity>())
    , last_update_failed_value(is.read_bool())
{
    std::copy(objects.begin(), objects.end(), std::inserter(dependencies_value,
                                                            dependencies_value.begin()));
    std::copy(libraries.begin(), libraries.end(), std::inserter(dependencies_value,
                                                                dependencies_value.begin()));
}

gorc::library_file_entity_internal_properties::library_file_entity_internal_properties(
        std::string const &library_name,
        ext_lib_set const &external_libs,
        std::unordered_set<object_file_entity*> const &objects,
        std::unordered_set<library_file_entity*> const &libraries)
    : library_name(library_name)
    , external_libs(external_libs)
    , objects(objects)
    , libraries(libraries)
{
    std::copy(objects.begin(), objects.end(), std::inserter(dependencies_value,
                                                            dependencies_value.begin()));
    std::copy(libraries.begin(), libraries.end(), std::inserter(dependencies_value,
                                                                dependencies_value.begin()));
}

gorc::library_file_entity_internal_properties::~library_file_entity_internal_properties()
{
    return;
}

gorc::library_file_entity::library_file_entity(entity_input_stream &is)
    : library_file_entity_internal_properties(is)
    , generated_file_entity(is.services.get<compiler_properties>()
                                       .make_library_filename(library_name))
{
    last_update_failed = last_update_failed_value;
    return;
}

gorc::library_file_entity::library_file_entity(std::string const &name,
                                               ext_lib_set const &external_libs,
                                               std::unordered_set<object_file_entity*> const &objs,
                                               std::unordered_set<library_file_entity*> const &lib,
                                               service_registry const &services)
    : library_file_entity_internal_properties(name, external_libs, objs, lib)
    , generated_file_entity(services.get<compiler_properties>()
                                    .make_library_filename(library_name))
{
    return;
}

std::unordered_set<gorc::entity*> const& gorc::library_file_entity::dependencies()
{
    return dependencies_value;
}

bool gorc::library_file_entity::update(service_registry const &services)
{
    bool rv = services.get<compiler_properties>().archive_static_library(this);
    last_update_failed = !rv;
    return rv;
}

void gorc::library_file_entity::serialize(entity_output_stream &os)
{
    os.write_string(library_name);
    os.write_enum_set(external_libs);
    os.write_entity_set(objects);
    os.write_entity_set(libraries);
    os.write_bool(last_update_failed);
}

gorc::ext_lib_set const& gorc::library_file_entity::get_external_libraries() const
{
    return external_libs;
}

std::unordered_set<gorc::object_file_entity*> const&
    gorc::library_file_entity::get_objects() const
{
    return objects;
}

std::unordered_set<gorc::library_file_entity*> const&
    gorc::library_file_entity::get_libraries() const
{
    return libraries;
}

std::string const& gorc::library_file_entity::get_library_name() const
{
    return library_name;
}

std::type_index gorc::library_file_entity::get_type_index() const
{
    return typeid(library_file_entity);
}
