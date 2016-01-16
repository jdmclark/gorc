#include "test/test.hpp"
#include "ast/node.hpp"
#include "log/log.hpp"

using namespace gorc;

namespace {

    class mock_node : public visitable_ast_node<mock_node> {
    public:
        mock_node()
            : visitable_ast_node<mock_node>(diagnostic_context_location())
        {
            return;
        }
    };

    class other_mock_node : public visitable_ast_node<other_mock_node> {
    public:
        other_mock_node()
            : visitable_ast_node<other_mock_node>(diagnostic_context_location())
        {
            return;
        }
    };

    class mock_node_ast_visitor {
    public:
        void visit(mock_node &) const
        {
            LOG_INFO("ast_visited mock_node");
        }

        void visit(other_mock_node &) const
        {
            LOG_INFO("ast_visited other_mock_node");
        }
    };

}

begin_suite(node_test);

test_case(simple_ast_visit)
{
    mock_node mn;
    other_mock_node omn;

    mock_node *mnp = &mn;
    other_mock_node *omnp = &omn;

    assert_log_empty();
    ast_visit(mock_node_ast_visitor(), mnp);
    assert_log_message(log_level::info, "<BUFFER>: ast_visited mock_node");
    assert_log_empty();

    ast_visit(mock_node_ast_visitor(), omnp);
    assert_log_message(log_level::info, "<BUFFER>: ast_visited other_mock_node");
    assert_log_empty();
}

test_case(variant_ast_visit)
{
    mock_node mn;
    other_mock_node omn;

    variant<mock_node*, other_mock_node*> mnp(&mn);
    variant<mock_node*, other_mock_node*> omnp(&omn);

    assert_log_empty();
    ast_visit(mock_node_ast_visitor(), mnp);
    assert_log_message(log_level::info, "<BUFFER>: ast_visited mock_node");
    assert_log_empty();

    ast_visit(mock_node_ast_visitor(), omnp);
    assert_log_message(log_level::info, "<BUFFER>: ast_visited other_mock_node");
    assert_log_empty();
}

end_suite(node_test);
