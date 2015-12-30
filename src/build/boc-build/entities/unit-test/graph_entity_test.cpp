#include "test/test.hpp"
#include "../graph_entity.hpp"

using namespace gorc;

begin_suite(graph_entity_test);

test_case(update_aborts)
{
    std::unordered_set<entity*> empty_project;
    graph_entity empty_graph(empty_project);

    assert_throws_logged(empty_graph.update(service_registry()));
    assert_log_message(log_level::error, "cannot update graph natively");
    assert_log_empty();
}

end_suite(graph_entity_test);
