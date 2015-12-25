#include "entity_closure.hpp"
#include "utility/maybe.hpp"
#include <unordered_map>

gorc::entity_closure gorc::compute_target_closure(
        std::unordered_set<entity*> const &root_targets)
{
    entity_closure closure;

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
