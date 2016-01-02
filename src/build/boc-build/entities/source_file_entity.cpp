#include "source_file_entity.hpp"
#include "build/boc-build/engine/entity_serializer.hpp"

void gorc::source_file_entity::serialize(entity_output_stream &os)
{
    os.write_path(filename);
    os.write_uint32(static_cast<uint32_t>(previous_timestamp));
}

std::type_index gorc::source_file_entity::get_type_index() const
{
    return typeid(source_file_entity);
}
