#include "test/test.hpp"
#include "build/boc-build/engine/dirty_list.hpp"
#include "mock_entity.hpp"

using namespace gorc;

begin_suite(dirty_list_test);

test_case(empty_closure)
{
    entity_closure empty;
    entity_adjacency_list empty_list(empty);
    dirty_entity_list dirty_list(empty, empty_list);

    assert_true(dirty_list.dirty_entities.empty());
    assert_true(dirty_list.dirty_leaf_entities.empty());
    assert_true(dirty_list.unsatisfiable_entities.empty());
}

test_case(simple_root)
{
    auto res = construct_mock_entity_graph({
            std::make_tuple("A", "B"),
            std::make_tuple("A", "C"),
            std::make_tuple("C", "D")
        },
        { "D" });

    std::unordered_set<gorc::entity*> initial_set { res.at("A").get() };

    auto result = compute_target_closure(initial_set);
    entity_adjacency_list adj_list(result);
    dirty_entity_list dirty_list(result, adj_list);

    entity_closure expected_dirty { res.at("A").get(),
                                    res.at("C").get(),
                                    res.at("D").get() };

    assert_eq(dirty_list.dirty_entities, expected_dirty);

    entity_closure expected_leaf { res.at("D").get() };

    assert_eq(dirty_list.dirty_leaf_entities, expected_leaf);

    assert_true(dirty_list.unsatisfiable_entities.empty());

    // Check dependency counts
    assert_eq(dirty_list.dirty_dependency_count[res.at("A").get()], size_t(1));
    assert_eq(dirty_list.dirty_dependency_count[res.at("C").get()], size_t(1));
    assert_eq(dirty_list.dirty_dependency_count[res.at("D").get()], size_t(0));
}

test_case(disjoint_subsets)
{
    auto res = construct_mock_entity_graph({
            std::make_tuple("A", "B"),
            std::make_tuple("A", "C"),
            std::make_tuple("A", "D"),

            std::make_tuple("B", "E"),
            std::make_tuple("B", "F"),
            std::make_tuple("F", "G"),

            std::make_tuple("C", "H"),
            std::make_tuple("C", "G")
        },
        { "F", "C" });

    std::unordered_set<gorc::entity*> initial_set { res.at("B").get(),
                                                    res.at("C").get() };

    auto result = compute_target_closure(initial_set);
    entity_adjacency_list adj_list(result);
    dirty_entity_list dirty_list(result, adj_list);

    entity_closure expected_dirty { res.at("B").get(),
                                    res.at("C").get(),
                                    res.at("F").get() };

    assert_eq(dirty_list.dirty_entities, expected_dirty);

    entity_closure expected_leaf { res.at("C").get(),
                                   res.at("F").get() };

    assert_eq(dirty_list.dirty_leaf_entities, expected_leaf);

    assert_true(dirty_list.unsatisfiable_entities.empty());

    // Check dependency counts
    assert_eq(dirty_list.dirty_dependency_count[res.at("B").get()], size_t(1));
    assert_eq(dirty_list.dirty_dependency_count[res.at("C").get()], size_t(0));
    assert_eq(dirty_list.dirty_dependency_count[res.at("F").get()], size_t(0));
}

test_case(cyclic)
{
    auto res = construct_mock_entity_graph({
            std::make_tuple("A", "B"),
            std::make_tuple("B", "C"),
            std::make_tuple("C", "D"),
            std::make_tuple("D", "B"),
            std::make_tuple("D", "E"),
            std::make_tuple("E", "F")
        },
        { "E" });

    std::unordered_set<gorc::entity*> initial_set { res.at("A").get() };

    auto result = compute_target_closure(initial_set);
    entity_adjacency_list adj_list(result);
    dirty_entity_list dirty_list(result, adj_list);

    entity_closure expected_dirty { res.at("E").get() };
    assert_eq(dirty_list.dirty_entities, expected_dirty);

    entity_closure expected_leaf { res.at("E").get() };
    assert_eq(dirty_list.dirty_leaf_entities, expected_dirty);

    entity_closure expected_unsat { res.at("A").get(),
                                    res.at("B").get(),
                                    res.at("C").get(),
                                    res.at("D").get() };
    assert_eq(dirty_list.unsatisfiable_entities, expected_unsat);
}

end_suite(dirty_list_test);
