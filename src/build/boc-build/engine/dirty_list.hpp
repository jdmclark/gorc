#pragma once

#include "adjacency_list.hpp"

namespace gorc {

    class dirty_entity_list {
    public:
        entity_closure dirty_entities;
        entity_closure dirty_leaf_entities;
        std::unordered_map<entity*, size_t> dirty_dependency_count;
        entity_closure unsatisfiable_entities;

        dirty_entity_list(entity_closure const &candidate_subgraph,
                          entity_adjacency_list const &edges);
    };

}
