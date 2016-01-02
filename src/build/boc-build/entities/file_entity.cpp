#include "file_entity.hpp"
#include "build/boc-build/engine/entity_serializer.hpp"
#include "build/boc-build/engine/entity_deserializer.hpp"
#include <boost/filesystem.hpp>

namespace {
    std::unordered_set<gorc::entity*> empty_dependencies;
}

gorc::file_entity::file_entity(entity_input_stream &is)
    : base_file_entity(is.read_path())
{
    previous_timestamp = static_cast<std::time_t>(is.read_uint32());
}

gorc::file_entity::file_entity(path const &new_filename,
                               std::time_t previous_timestamp)
    : base_file_entity(new_filename)
    , previous_timestamp(previous_timestamp)
{
    return;
}

std::unordered_set<gorc::entity*> const& gorc::file_entity::dependencies()
{
    return empty_dependencies;
}

bool gorc::file_entity::is_dirty()
{
    return previous_timestamp < boost::filesystem::last_write_time(filename);
}

bool gorc::file_entity::update(gorc::service_registry const &)
{
    previous_timestamp = boost::filesystem::last_write_time(filename);
    return true;
}

void gorc::file_entity::serialize(entity_output_stream &os)
{
    os.write_path(filename);
    os.write_uint32(static_cast<uint32_t>(previous_timestamp));
}

std::type_index gorc::file_entity::get_type_index() const
{
    return typeid(file_entity);
}
