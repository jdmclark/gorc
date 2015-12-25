#include "dirty_list.hpp"
#include "utility/range.hpp"

gorc::dirty_entity_list::dirty_entity_list(entity_closure const &candidate_subgraph,
                                           entity_adjacency_list const &edges)
{
    std::unordered_set<entity*> &pending_nodes = unsatisfiable_entities;
    std::unordered_set<entity*> open_nodes;

    std::unordered_map<entity*, size_t> dependencies_satisfied;

    // Pre-sort nodes into leaves and interior nodes
    for(entity *ent : candidate_subgraph) {
        dependencies_satisfied.emplace(ent, 0);
        dirty_dependency_count.emplace(ent, 0);

        if(ent->dependencies().empty()) {
            open_nodes.insert(ent);
        }
        else {
            pending_nodes.insert(ent);
        }
    }

    // Continue processing until all nodes are satisfied
    while(!open_nodes.empty()) {
        auto it = open_nodes.begin();
        entity *open_node = *it;
        open_nodes.erase(it);

        // Dirty check may be expensive. Only do it if there are no dirty dependencies.
        bool open_node_is_dirty =
            (dirty_dependency_count[open_node] != 0) ||
            open_node->is_dirty();

        if(open_node_is_dirty) {
            dirty_entities.insert(open_node);

            if(dirty_dependency_count[open_node] == 0) {
                dirty_leaf_entities.insert(open_node);
            }
        }

        for(auto const &dependent : make_range(edges.dependents.equal_range(open_node))) {
            if(open_node_is_dirty) {
                ++dirty_dependency_count[dependent.second];
            }

            if(dependent.second->dependencies().size() ==
               (++dependencies_satisfied[dependent.second])) {
                // Dependent has been fully satisifed
                open_nodes.insert(dependent.second);
                pending_nodes.erase(dependent.second);
            }
        }
    }

    return;
}
