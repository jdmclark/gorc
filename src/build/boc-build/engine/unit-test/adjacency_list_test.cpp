#include "test/test.hpp"
#include "build/boc-build/engine/adjacency_list.hpp"
#include "mock_entity.hpp"

using namespace gorc;

namespace {

    using named_edge_list = std::set<std::tuple<std::string, std::string>>;

    named_edge_list convert_edge_list(entity_edge_list const &edges)
    {
        std::set<std::tuple<std::string, std::string>> rv;
        for(auto const &edge : edges) {
            rv.emplace(edge.first->name(), edge.second->name());
        }

        return rv;
    }

}

begin_suite(adjacency_list_test);

test_case(empty_closure)
{
    entity_closure empty;
    entity_adjacency_list empty_list(empty);
    assert_true(empty_list.dependencies.empty());
    assert_true(empty_list.dependents.empty());
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
    entity_adjacency_list adj_list(result);

    named_edge_list expected_dependencies {
        std::make_tuple("A", "B"),
        std::make_tuple("A", "C"),
        std::make_tuple("C", "D") };

    assert_eq(expected_dependencies, convert_edge_list(adj_list.dependencies));

    named_edge_list expected_dependents {
        std::make_tuple("B", "A"),
        std::make_tuple("C", "A"),
        std::make_tuple("D", "C") };

    assert_eq(expected_dependents, convert_edge_list(adj_list.dependents));
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
    entity_adjacency_list adj_list(result);

    named_edge_list expected_dependencies {
        std::make_tuple("B", "E"),
        std::make_tuple("B", "F"),
        std::make_tuple("F", "G"),
        std::make_tuple("C", "H"),
        std::make_tuple("C", "G") };

    assert_eq(expected_dependencies, convert_edge_list(adj_list.dependencies));

    named_edge_list expected_dependents {
        std::make_tuple("E", "B"),
        std::make_tuple("F", "B"),
        std::make_tuple("G", "F"),
        std::make_tuple("H", "C"),
        std::make_tuple("G", "C") };

    assert_eq(expected_dependents, convert_edge_list(adj_list.dependents));
}

end_suite(adjacency_list_test);
