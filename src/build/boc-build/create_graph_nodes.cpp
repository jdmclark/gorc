#include "create_graph_nodes.hpp"

#include "entities/file_entity.hpp"
#include "entities/source_file_entity.hpp"
#include "entities/graph_entity.hpp"
#include "entities/library_file_entity.hpp"
#include "entities/source_file_entity.hpp"
#include "entities/object_file_entity.hpp"
#include "entities/program_file_entity.hpp"

#include <boost/filesystem.hpp>

gorc::root_entity* gorc::create_graph_nodes(project_file const &pf,
                                            entity_allocator &ea)
{
    // Create graph nodes
    std::unordered_set<entity*> project_files;
    for(auto const &subfile : pf.project_files) {
        project_files.insert(ea.emplace<file_entity>(
                    subfile,
                    boost::filesystem::last_write_time(subfile)));
    }

    graph_entity *graph_ent = ea.emplace<graph_entity>(project_files);
    return ea.emplace<root_entity>(graph_ent);
}
