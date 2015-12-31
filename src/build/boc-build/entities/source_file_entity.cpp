#include "source_file_entity.hpp"
#include "build/boc-build/engine/entity_serializer.hpp"

void gorc::source_file_entity::serialize(entity_output_stream &os)
{
    os.write_entity_type_id<source_file_entity>();
    os.write_path(filename);
    os.write_uint32(static_cast<uint32_t>(previous_timestamp));
}
