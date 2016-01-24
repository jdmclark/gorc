#include "program_file_entity.hpp"
#include "build/boc-build/engine/entity_serializer.hpp"
#include "build/boc-build/engine/entity_deserializer.hpp"
#include "compiler_properties.hpp"
#include <boost/filesystem.hpp>

gorc::program_file_entity_internal_properties::program_file_entity_internal_properties(
        entity_input_stream &is)
    : program_name(is.read_string())
    , type(static_cast<program_type>(is.read_uint32()))
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

gorc::program_file_entity_internal_properties::program_file_entity_internal_properties(
        std::string const &program_name,
        program_type type,
        ext_lib_set const &ext_libs,
        std::unordered_set<object_file_entity*> const &objs,
        std::unordered_set<library_file_entity*> const &libs)
    : program_name(program_name)
    , type(type)
    , external_libs(ext_libs)
    , objects(objs)
    , libraries(libs)
{
    std::copy(objects.begin(), objects.end(), std::inserter(dependencies_value,
                                                            dependencies_value.begin()));
    std::copy(libraries.begin(), libraries.end(), std::inserter(dependencies_value,
                                                                dependencies_value.begin()));
}

gorc::program_file_entity_internal_properties::~program_file_entity_internal_properties()
{
    return;
}

gorc::program_file_entity::program_file_entity(entity_input_stream &is)
    : program_file_entity_internal_properties(is)
    , generated_file_entity(is.services.get<compiler_properties>()
                                       .make_program_filename(program_name, type))
{
    last_update_failed = last_update_failed_value;
    return;
}

gorc::program_file_entity::program_file_entity(std::string const &program_name,
                                               program_type type,
                                               ext_lib_set const &ext_libs,
                                               std::unordered_set<object_file_entity*> const &objs,
                                               std::unordered_set<library_file_entity*> const &libs,
                                               service_registry const &services)
    : program_file_entity_internal_properties(program_name,
                                              type,
                                              ext_libs,
                                              objs,
                                              libs)
    , generated_file_entity(services.get<compiler_properties>()
                                    .make_program_filename(program_name, type))
{
    return;
}

std::unordered_set<gorc::entity*> const& gorc::program_file_entity::dependencies()
{
    return dependencies_value;
}

bool gorc::program_file_entity::update(service_registry const &services)
{
    bool rv = services.get<compiler_properties>().link_program(this);
    last_update_failed = !rv;
    return rv;
}

void gorc::program_file_entity::serialize(entity_output_stream &os)
{
    os.write_string(program_name);
    os.write_uint32(static_cast<uint32_t>(type));
    os.write_enum_set(external_libs);
    os.write_entity_set(objects);
    os.write_entity_set(libraries);
    os.write_bool(last_update_failed);
}

std::type_index gorc::program_file_entity::get_type_index() const
{
    return typeid(program_file_entity);
}

gorc::ext_lib_set const& gorc::program_file_entity::get_external_libraries() const
{
    return external_libs;
}

std::unordered_set<gorc::object_file_entity*> const&
    gorc::program_file_entity::get_objects() const
{
    return objects;
}

std::unordered_set<gorc::library_file_entity*> const&
    gorc::program_file_entity::get_libraries() const
{
    return libraries;
}
