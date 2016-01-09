#include "print_status.hpp"

#include "engine/dirty_list.hpp"
#include "engine/adjacency_list.hpp"
#include "engine/entity_closure.hpp"
#include "engine/entity_scheduler.hpp"

#include "entities/root_entity.hpp"

#include <map>
#include <algorithm>
#include <iostream>

void gorc::print_status(root_entity *root)
{
    // Find all out-of-date targest for now.
    // In the future, may want status from a subdirectory.
    std::unordered_set<entity*> initial_set { root->project };

    entity_closure closure = compute_target_closure(initial_set);
    entity_adjacency_list edges(closure);
    dirty_entity_list dirty(closure, edges);
    entity_scheduler scheduler(dirty, edges);

    std::map<std::string, entity*> dirty_entities;
    std::transform(dirty.dirty_entities.begin(),
                   dirty.dirty_entities.end(),
                   std::inserter(dirty_entities, dirty_entities.begin()),
                   [](entity *ent) -> std::pair<std::string, entity*> {
                        return std::make_pair(ent->name(), ent); });

    if(dirty_entities.empty()) {
        std::cout << "All targets are up-to-date." << std::endl;
    }
    else {
        std::cout << "Out-of-date targets:" << std::endl;
        for(auto const &ent : dirty_entities) {
            std::cout << "    " << std::get<0>(ent) << std::endl;
        }
    }

    return;
}
