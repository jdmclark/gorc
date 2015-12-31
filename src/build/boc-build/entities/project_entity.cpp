#include "project_entity.hpp"
#include "build/boc-build/engine/entity_serializer.hpp"
#include "build/boc-build/engine/entity_deserializer.hpp"

namespace {
    std::string project_name = "project";
}

gorc::project_entity::project_entity(entity_input_stream &is)
    : project_entity(is.read_entity_set<program_file_entity>())
{
    return;
}

void gorc::project_entity::serialize(entity_output_stream &os)
{
    os.write_entity_type_id<project_entity>();
    os.write_entity_set(programs);
}

gorc::project_entity::project_entity(std::unordered_set<program_file_entity*> const &programs)
    : programs(programs)
{
    std::copy(programs.begin(), programs.end(), std::inserter(dependencies_value,
                                                              dependencies_value.begin()));
}

std::string const& gorc::project_entity::name()
{
    return project_name;
}

std::unordered_set<gorc::entity*> const& gorc::project_entity::dependencies()
{
    return dependencies_value;
}

bool gorc::project_entity::is_dirty()
{
    return false;
}

bool gorc::project_entity::update(service_registry const &)
{
    return true;
}
