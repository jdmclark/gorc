#pragma once

#include "dirty_list.hpp"
#include "adjacency_list.hpp"
#include <mutex>

namespace gorc {

    class entity_scheduler {
    private:
        dirty_entity_list const &dirty_list;
        entity_adjacency_list const &edges;

        std::unordered_map<entity*, size_t> dirty_dependency_count;

        std::unordered_set<entity*> pending_entities;
        std::unordered_set<entity*> issued_entities;

        std::unordered_set<entity*> ready_entities;

        std::unordered_set<entity*> unsatisfiable_entities;
        std::unordered_set<entity*> failed_entities;

    public:
        entity_scheduler(dirty_entity_list const &dirty_list,
                         entity_adjacency_list const &edges);

        bool exhausted();
        bool done();
        bool succeeded();

        bool await();
        entity* issue();

        void retire(entity*, bool successful);
    };

}
