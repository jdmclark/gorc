#include "run_build.hpp"

#include "engine/dirty_list.hpp"
#include "engine/adjacency_list.hpp"
#include "engine/entity_closure.hpp"
#include "engine/entity_scheduler.hpp"

#include "entities/root_entity.hpp"

#include "log/log.hpp"

int gorc::run_build(service_registry const &services, root_entity *root)
{
    // Build everything for now.
    // TODO: Need to prune this based on original working directory.
    std::unordered_set<gorc::entity*> initial_set { root->project };

    entity_closure closure = compute_target_closure(initial_set);
    entity_adjacency_list edges(closure);
    dirty_entity_list dirty(closure, edges);
    entity_scheduler scheduler(dirty, edges);

    // TODO: Multithreading
    while(!scheduler.done()) {
        if(scheduler.await()) {
            LOG_ERROR("Build stalled");
            break;
        }

        entity *curr = scheduler.issue();
        bool succeeded = curr->update(services);
        scheduler.retire(curr, succeeded);
    }

    if(scheduler.succeeded()) {
        LOG_INFO("Build succeeded");
        return EXIT_SUCCESS;
    }
    else {
        LOG_ERROR("Build failed");
        return EXIT_FAILURE;
    }
}
