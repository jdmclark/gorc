#include "run_build.hpp"

#include "engine/dirty_list.hpp"
#include "engine/adjacency_list.hpp"
#include "engine/entity_closure.hpp"
#include "engine/entity_scheduler.hpp"

#include "entities/root_entity.hpp"

#include "utility/progress.hpp"
#include "log/log.hpp"

#include <set>
#include <map>
#include <algorithm>
#include <iostream>

namespace {

    void print_entity_set(std::string const &name, std::unordered_set<gorc::entity*> const &es)
    {
        if(es.empty()) {
            return;
        }

        std::cout << name << ":" << std::endl;

        std::multimap<std::string, gorc::entity*> values;
        std::transform(es.begin(),
                       es.end(),
                       std::inserter(values, values.begin()),
                       [](gorc::entity *ent) -> std::pair<std::string, gorc::entity*> {
                            return std::make_pair(ent->name(), ent); });

        for(auto const &value : values) {
            std::cout << "    " << value.first << std::endl;
        }
    }

    void print_summary(gorc::entity_scheduler const &es)
    {
        print_entity_set("Succeeded", es.get_succeeded_entities());
        print_entity_set("Failed", es.get_failed_entities());
        print_entity_set("Unsatisfiable", es.get_unsatisfiable_entities());
    }

}

int gorc::run_build(service_registry const &services, root_entity *root, bool needs_summary)
{
    // Build everything for now.
    // TODO: Need to prune this based on original working directory.
    std::unordered_set<gorc::entity*> initial_set { root->project };

    entity_closure closure = compute_target_closure(initial_set);
    entity_adjacency_list edges(closure);
    dirty_entity_list dirty(closure, edges);
    entity_scheduler scheduler(dirty, edges);

    if(dirty.dirty_entities.empty()) {
        LOG_INFO("All targets are up-to-date");
        return EXIT_SUCCESS;
    }

    LOG_INFO(format("Building %d out-of-date targets") % dirty.dirty_entities.size());
    auto pbar = services.get<progress_factory>().make_progress(dirty.dirty_entities.size());

    // TODO: Multithreading
    while(!scheduler.done()) {
        if(scheduler.await()) {
            LOG_ERROR("Build stalled");
            break;
        }

        entity *curr = scheduler.issue();
        bool succeeded = curr->update(services);
        scheduler.retire(curr, succeeded);
        pbar->advance();
    }

    pbar->finished();

    if(scheduler.succeeded()) {
        LOG_INFO("Build succeeded");
    }
    else {
        needs_summary = true;
        LOG_ERROR("Build failed");
    }

    if(needs_summary) {
        print_summary(scheduler);
    }

    return scheduler.succeeded() ? EXIT_SUCCESS : EXIT_FAILURE;
}
