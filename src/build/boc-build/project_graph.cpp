#include "project_graph.hpp"
#include "project_file.hpp"
#include "engine/entity_serializer.hpp"
#include "engine/entity_deserializer.hpp"
#include "engine/dirty_list.hpp"
#include "engine/entity_closure.hpp"
#include "engine/adjacency_list.hpp"
#include "io/native_file.hpp"
#include "log/log.hpp"
#include "entities/root_entity.hpp"
#include "entities/graph_entity.hpp"
#include "entities/file_entity.hpp"
#include <boost/filesystem.hpp>

gorc::project_graph::project_graph(entity_registry const &reg,
                                   path const &project_filename,
                                   path const &cache_filename)
    : reg(reg)
    , cache_filename(cache_filename)
{
    if(boost::filesystem::is_regular_file(cache_filename)) {
        // Load from the cache file instead of the project files (fast)
        LOG_INFO("Loading dependency graph from cache");

        auto nf = make_native_read_only_file(cache_filename);
        entity_deserializer ed(reg, ea, *nf);

        root = ed.deserialize_root<root_entity>();

        // Check to see if the graph sub-entity requires updating.
        std::unordered_set<entity*> subgraph { root->graph };
        auto subgraph_closure = compute_target_closure(subgraph);
        entity_adjacency_list adj_list(subgraph_closure);
        dirty_entity_list dirty_list(subgraph_closure, subgraph_closure);

        if(dirty_list.dirty_entities.empty()) {
            // Graph is up-to-date
            return;
        }

        LOG_INFO("Cached dependency graph is out-of-date");
    }

    // Load from the project files (slow)
    LOG_INFO("Loading dependency graph from project files");

    ea.clear();

    project_file pf(project_filename);

    // Create graph nodes
    std::unordered_set<entity*> project_files;
    for(auto const &subfile : pf.project_files) {
        project_files.insert(ea.emplace<file_entity>(
                    subfile,
                    boost::filesystem::last_write_time(subfile)));
    }

    graph_entity *graph_ent = ea.emplace<graph_entity>(project_files);
    root = ea.emplace<root_entity>(graph_ent);

    LOG_INFO("Dependency graph generated");
}

gorc::project_graph::~project_graph()
{
    LOG_INFO("Caching dependency graph");

    std::unique_ptr<output_stream> nf;
    try {
        nf = make_native_file(cache_filename);
    }
    catch(std::exception const &e) {
        LOG_ERROR(format("Cannot write cache to file %s: %s") %
                  cache_filename.generic_string() %
                  e.what());
        return;
    }

    entity_serializer es(reg, *nf);
    es.serialize_root(root);

    LOG_INFO("Finished writing cache");
}

gorc::root_entity* gorc::project_graph::get_root()
{
    return root;
}
