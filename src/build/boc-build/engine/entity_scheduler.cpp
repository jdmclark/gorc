#include "entity_scheduler.hpp"
#include "utility/range.hpp"

gorc::entity_scheduler::entity_scheduler(dirty_entity_list const &dirty_list,
                                         entity_adjacency_list const &edges)
    : dirty_list(dirty_list)
    , edges(edges)
{
    dirty_dependency_count = dirty_list.dirty_dependency_count;

    for(entity *dirty_ent : dirty_list.dirty_entities) {
        if(dirty_list.dirty_leaf_entities.find(dirty_ent) !=
           dirty_list.dirty_leaf_entities.end()) {
            ready_entities.insert(dirty_ent);
        }
        else {
            pending_entities.insert(dirty_ent);
        }
    }
}

bool gorc::entity_scheduler::exhausted()
{
    return ready_entities.empty() && pending_entities.empty();
}

bool gorc::entity_scheduler::done()
{
    return issued_entities.empty() && exhausted();
}

bool gorc::entity_scheduler::succeeded()
{
    return failed_entities.empty();
}

bool gorc::entity_scheduler::await()
{
    return ready_entities.empty();
}

gorc::entity* gorc::entity_scheduler::issue()
{
    auto it = ready_entities.begin();
    entity *rv = *it;
    ready_entities.erase(it);

    // Mark entity as posted for processing
    issued_entities.insert(rv);

    return rv;
}

namespace {
    void retire_parents_unsatisfiable(gorc::entity *ent,
                                      std::unordered_set<gorc::entity*> &unsat,
                                      std::unordered_set<gorc::entity*> &pending,
                                      gorc::entity_adjacency_list const &edges)
    {
        for(auto const &dep : make_range(edges.dependents.equal_range(ent))) {
            pending.erase(dep.second);
            if(unsat.find(dep.second) == unsat.end()) {
                unsat.insert(dep.second);
                retire_parents_unsatisfiable(dep.second,
                                             unsat,
                                             pending,
                                             edges);
            }
        }
    }
}

void gorc::entity_scheduler::retire(gorc::entity *done_ent, bool successful)
{
    // Mark entity as completed
    issued_entities.erase(done_ent);

    if(successful) {
        succeeded_entities.insert(done_ent);

        // Decrement dependency counts of all dependents.
        for(auto const &dep : make_range(edges.dependents.equal_range(done_ent))) {
            --dirty_dependency_count[dep.second];
            if(dirty_dependency_count[dep.second] == 0) {
                // Dependent is unblocked for processing.
                pending_entities.erase(dep.second);
                ready_entities.insert(dep.second);
            }
        }
    }
    else {
        failed_entities.insert(done_ent);
        retire_parents_unsatisfiable(done_ent, unsatisfiable_entities, pending_entities, edges);
    }
}

std::unordered_set<gorc::entity*> const& gorc::entity_scheduler::get_unsatisfiable_entities() const
{
    return unsatisfiable_entities;
}

std::unordered_set<gorc::entity*> const& gorc::entity_scheduler::get_failed_entities() const
{
    return failed_entities;
}

std::unordered_set<gorc::entity*> const& gorc::entity_scheduler::get_succeeded_entities() const
{
    return succeeded_entities;
}
