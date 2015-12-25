#include "adjacency_list.hpp"

gorc::entity_adjacency_list::entity_adjacency_list(entity_closure const &closure)
{
    for(auto const &entity : closure) {
        for(auto const &dep : entity->dependencies()) {
            dependencies.emplace(entity, dep);
            dependents.emplace(dep, entity);
        }
    }
}
