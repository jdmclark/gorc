#include "test/test.hpp"
#include "build/boc-build/engine/entity_scheduler.hpp"
#include "mock_entity.hpp"
#include "log/log.hpp"

using namespace gorc;

namespace {

    void run_build(entity_scheduler &sched)
    {
        while(!sched.done()) {
            if(sched.await()) {
                LOG_ERROR("build stalled!");
                return;
            }

            entity *curr = sched.issue();
            bool succeeded = curr->update();
            sched.retire(curr, succeeded);
        }

        if(sched.succeeded()) {
            LOG_INFO("build succeeded");
        }
        else {
            LOG_INFO("build failed");
        }
    }

}

begin_suite(entity_scheduler_test);

test_case(empty_closure)
{
    entity_closure closure;
    entity_adjacency_list edges(closure);
    dirty_entity_list dirty(closure, edges);
    entity_scheduler scheduler(dirty, edges);

    assert_true(scheduler.done());
    assert_true(scheduler.exhausted());
    assert_true(scheduler.succeeded());

    assert_log_empty();
}

test_case(simple_pending)
{
    auto res = construct_mock_entity_graph({
            std::make_tuple("A", "B"),
            std::make_tuple("B", "C")
        },

        /* Dirty */
        { "B" });

    std::unordered_set<gorc::entity*> initial_set { res.at("A").get() };

    entity_closure closure = compute_target_closure(initial_set);
    entity_adjacency_list edges(closure);
    dirty_entity_list dirty(closure, edges);
    entity_scheduler scheduler(dirty, edges);

    assert_log_empty();

    assert_true(!scheduler.done());
    assert_true(!scheduler.exhausted());
    assert_true(!scheduler.await());

    entity *expected_b = scheduler.issue();

    assert_true(!scheduler.done());
    assert_true(!scheduler.exhausted());
    assert_true(scheduler.await());
    assert_eq(expected_b->name(), std::string("B"));

    scheduler.retire(expected_b, true);

    assert_true(!scheduler.done());
    assert_true(!scheduler.exhausted());
    assert_true(!scheduler.await());

    entity *expected_a = scheduler.issue();

    assert_true(!scheduler.done());
    assert_true(scheduler.exhausted());
    assert_true(scheduler.await());
    assert_eq(expected_a->name(), std::string("A"));

    scheduler.retire(expected_a, true);

    assert_true(scheduler.done());
    assert_true(scheduler.exhausted());
    assert_true(scheduler.succeeded());

    assert_log_empty();
}

test_case(simple_pending_automatic_build)
{
    auto res = construct_mock_entity_graph({
            std::make_tuple("A", "B"),
            std::make_tuple("B", "C")
        },

        /* Dirty */
        { "B" });

    std::unordered_set<gorc::entity*> initial_set { res.at("A").get() };

    entity_closure closure = compute_target_closure(initial_set);
    entity_adjacency_list edges(closure);
    dirty_entity_list dirty(closure, edges);
    entity_scheduler scheduler(dirty, edges);

    assert_log_empty();
    run_build(scheduler);

    assert_log_message(log_level::info, "B build succeeded");
    assert_log_message(log_level::info, "A build succeeded");
    assert_log_message(log_level::info, "build succeeded");
    assert_log_empty();
}

test_case(unsatisfiable_entry)
{
    auto res = construct_mock_entity_graph({
            std::make_tuple("A", "B"),
            std::make_tuple("B", "C")
        },

        /* Dirty */
        { "C" },

        /* Failing */
        { "B" });

    std::unordered_set<gorc::entity*> initial_set { res.at("A").get() };

    entity_closure closure = compute_target_closure(initial_set);
    entity_adjacency_list edges(closure);
    dirty_entity_list dirty(closure, edges);
    entity_scheduler scheduler(dirty, edges);

    assert_log_empty();
    run_build(scheduler);

    assert_log_message(log_level::info, "C build succeeded");
    assert_log_message(log_level::info, "B build failed");
    assert_log_message(log_level::info, "build failed");
    assert_log_empty();
}

test_case(diamond)
{
    auto res = construct_mock_entity_graph({
            std::make_tuple("A", "B"),
            std::make_tuple("B", "C"),
            std::make_tuple("A", "C")
        },

        /* Dirty */
        { "A", "B", "C" });

    std::unordered_set<gorc::entity*> initial_set { res.at("A").get() };

    entity_closure closure = compute_target_closure(initial_set);
    entity_adjacency_list edges(closure);
    dirty_entity_list dirty(closure, edges);
    entity_scheduler scheduler(dirty, edges);

    assert_log_empty();
    run_build(scheduler);

    assert_log_message(log_level::info, "C build succeeded");
    assert_log_message(log_level::info, "B build succeeded");
    assert_log_message(log_level::info, "A build succeeded");
    assert_log_message(log_level::info, "build succeeded");
    assert_log_empty();
}

end_suite(entity_scheduler_test);
