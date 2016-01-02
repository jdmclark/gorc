#include "root_entity.hpp"
#include "log/log.hpp"
#include "build/boc-build/engine/entity_serializer.hpp"
#include "build/boc-build/engine/entity_deserializer.hpp"

namespace {
    std::string root_name = "root";
}

gorc::root_entity::root_entity(entity_input_stream &is)
    : graph(is.read_entity_reference<graph_entity>())
    , project(is.read_entity_reference<project_entity>())
{
    dependencies_value.insert(graph);
    dependencies_value.insert(project);
    return;
}

void gorc::root_entity::serialize(entity_output_stream &os)
{
    os.write_entity_reference(graph);
    os.write_entity_reference(project);
}

gorc::root_entity::root_entity(graph_entity *graph,
                               project_entity *project)
    : graph(graph)
    , project(project)
{
    dependencies_value.insert(graph);
    dependencies_value.insert(project);
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

std::type_index gorc::root_entity::get_type_index() const
{
    return typeid(root_entity);
}
