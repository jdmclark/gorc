#include "entity_closure.hpp"
#include "utility/maybe.hpp"
#include <unordered_map>

std::unordered_set<gorc::entity*> gorc::compute_target_closure(
        std::unordered_set<entity*> const &root_targets)
{
    std::unordered_set<entity*> closure;

    std::unordered_set<entity*> open = root_targets;

    while(!open.empty()) {
        // Pull entity from the open set
        auto it = open.begin();
        entity *open_entity = *it;
        open.erase(it);

        if(!closure.emplace(open_entity).second) {
            // Open entity is already in the closed set. Skip.
            continue;
        }

        for(entity *dep : open_entity->dependencies()) {
            open.emplace(dep);
        }
    }

    return closure;
}
