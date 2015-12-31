#include "test/test.hpp"
#include "../root_entity.hpp"
#include "../graph_entity.hpp"

using namespace gorc;

begin_suite(root_entity_test);

test_case(update_aborts)
{
    std::unordered_set<entity*> empty_project;
    graph_entity empty_graph(empty_project);

    std::unordered_set<program_file_entity*> empty_progs;
    project_entity empty_proj(empty_progs);

    root_entity empty_root(&empty_graph, &empty_proj);

    assert_throws_logged(empty_root.update(service_registry()));
    assert_log_message(log_level::error, "cannot update root natively");
    assert_log_empty();
}

test_case(constant_properties)
{
    std::unordered_set<entity*> empty_project;
    graph_entity empty_graph(empty_project);

    std::unordered_set<program_file_entity*> empty_progs;
    project_entity empty_proj(empty_progs);

    root_entity empty_root(&empty_graph, &empty_proj);

    assert_eq(empty_root.name(), "root");
    assert_true(!empty_root.is_dirty());
}

end_suite(root_entity_test);
