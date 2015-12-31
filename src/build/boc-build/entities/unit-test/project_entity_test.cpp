#include "test/test.hpp"
#include "../project_entity.hpp"

using namespace gorc;

begin_suite(project_entity_test);

test_case(name)
{
    std::unordered_set<program_file_entity*> empty_project;
    project_entity empty_graph(empty_project);

    assert_eq(empty_graph.name(), std::string("project"));
}

end_suite(project_entity_test);
