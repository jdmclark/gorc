#include "graph_entity.hpp"
#include "log/log.hpp"
#include "build/boc-build/engine/entity_serializer.hpp"
#include "build/boc-build/engine/entity_deserializer.hpp"

namespace {
    std::string graph_name = "graph";
}

gorc::graph_entity::graph_entity(entity_input_stream &is)
{
    uint32_t num_files = is.read_uint32();
    for(uint32_t i = 0; i < num_files; ++i) {
        project_files.insert(is.read_abstract_entity_reference());
    }
}

void gorc::graph_entity::serialize(entity_output_stream &os)
{
    os.write_entity_type_id<graph_entity>();
    os.write_uint32(static_cast<uint32_t>(project_files.size()));
    for(entity *file : project_files) {
        os.write_entity_reference(file);
    }
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
