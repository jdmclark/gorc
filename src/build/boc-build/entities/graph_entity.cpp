#include "graph_entity.hpp"
#include "log/log.hpp"
#include "build/boc-build/engine/entity_serializer.hpp"
#include "build/boc-build/engine/entity_deserializer.hpp"

namespace {
    std::string graph_name = "graph";
}

gorc::graph_entity::graph_entity(entity_input_stream &is)
    : project_files(is.read_entity_set<entity>())
{
    return;
}

void gorc::graph_entity::serialize(entity_output_stream &os)
{
    os.write_entity_set(project_files);
}

gorc::graph_entity::graph_entity(std::unordered_set<entity*> const &project_files)
    : project_files(project_files)
{
    return;
}

std::string const& gorc::graph_entity::name()
{
    return graph_name;
}

std::unordered_set<gorc::entity*> const& gorc::graph_entity::dependencies()
{
    return project_files;
}

bool gorc::graph_entity::is_dirty()
{
    return false;
}

bool gorc::graph_entity::update(service_registry const &)
{
    // Graph entity cannot be updated internally via the engine.
    // Updating the graph requires purging the existing graph.
    LOG_FATAL("cannot update graph natively");
}

std::type_index gorc::graph_entity::get_type_index() const
{
    return typeid(graph_entity);
}
