#include "root_entity.hpp"
#include "log/log.hpp"
#include "build/boc-build/engine/entity_serializer.hpp"
#include "build/boc-build/engine/entity_deserializer.hpp"

namespace {
    std::string root_name = "root";
}

gorc::root_entity::root_entity(entity_input_stream &is)
    : graph(is.read_entity_reference<graph_entity>())
{
    dependencies_value.insert(graph);
    return;
}

void gorc::root_entity::serialize(entity_output_stream &os)
{
    os.write_entity_type_id<root_entity>();
    os.write_entity_reference(graph);
}

gorc::root_entity::root_entity(graph_entity *graph)
    : graph(graph)
{
    dependencies_value.insert(graph);
    return;
}

std::string const& gorc::root_entity::name()
{
    return root_name;
}

std::unordered_set<gorc::entity*> const& gorc::root_entity::dependencies()
{
    return dependencies_value;
}

bool gorc::root_entity::is_dirty()
{
    return false;
}

bool gorc::root_entity::update(service_registry const &)
{
    // Root entity cannot be updated internally via the engine.
    // Updating the root requires purging the existing graph.
    LOG_FATAL("cannot update root natively");
}
