#include "object_file_entity.hpp"
#include "build/boc-build/engine/entity_serializer.hpp"
#include "build/boc-build/engine/entity_deserializer.hpp"
#include "compiler_properties.hpp"
#include <boost/filesystem.hpp>

gorc::object_file_entity::object_file_entity(entity_input_stream &is)
    : object_file_entity(is.read_entity_reference<source_file_entity>(),
                         is.services)
{
    last_update_failed = is.read_bool();
    return;
}

gorc::object_file_entity::object_file_entity(source_file_entity *primary_source_file,
                                             service_registry const &services)
    : generated_file_entity(services.get<compiler_properties>()
                                .make_object_filename_from_psf(primary_source_file->file_path()))
    , primary_source_file(primary_source_file)
{
    dependencies_value.insert(primary_source_file);
}

std::unordered_set<gorc::entity*> const& gorc::object_file_entity::dependencies()
{
    return dependencies_value;
}

bool gorc::object_file_entity::update(service_registry const &services)
{
    auto &comp_props = services.get<compiler_properties>();

    bool rv = comp_props.compile_object_file(this);
    last_update_failed = !rv;
    return rv;
}

void gorc::object_file_entity::serialize(entity_output_stream &os)
{
    os.write_entity_reference(primary_source_file);
    os.write_bool(last_update_failed);
}

std::type_index gorc::object_file_entity::get_type_index() const
{
    return typeid(object_file_entity);
}
