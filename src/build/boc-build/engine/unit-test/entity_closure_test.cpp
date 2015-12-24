#include "test/test.hpp"
#include "build/boc-build/engine/entity_closure.hpp"
#include "mock_entity.hpp"

using namespace gorc;

begin_suite(entity_closure_test);

test_case(empty_closure)
{
    auto res = compute_target_closure(std::unordered_set<entity*>());
    assert_true(res.empty());
}

test_case(simple_root)
{
    auto res = construct_mock_entity_graph({
            std::make_tuple("A", "B"),
            std::make_tuple("A", "C"),
            std::make_tuple("C", "D")
        });

    std::unordered_set<gorc::entity*> initial_set { res.at("A").get() };

    auto result = compute_target_closure(initial_set);

    std::set<std::string> expected {
        "A", "B", "C", "D" };

    assert_eq(mock_entities_to_strings(result), expected);
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
        });

    std::unordered_set<gorc::entity*> initial_set { res.at("B").get(),
                                                    res.at("C").get() };

    auto result = compute_target_closure(initial_set);

    std::set<std::string> expected {
        "B", "C", "E", "F", "G", "H" };

    assert_eq(mock_entities_to_strings(result), expected);
}

end_suite(entity_closure_test);
