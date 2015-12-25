#pragma once

#include "entity_closure.hpp"
#include <unordered_map>

namespace gorc {

    using entity_edge_list = std::unordered_multimap<entity*, entity*>;

    class entity_adjacency_list {
    public:
        entity_adjacency_list(entity_closure const &);

        entity_edge_list dependencies;
        entity_edge_list dependents;
    };

}
