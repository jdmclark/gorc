#include "test/test.hpp"
#include "cog/ast/node.hpp"
#include "log/log.hpp"

using namespace gorc;
using namespace gorc::cog;
using namespace gorc::cog::ast;

class mock_node : public visitable_node<mock_node> {
public:
    mock_node()
        : visitable_node<mock_node>(diagnostic_context_location())
    {
        return;
    }
};

class other_mock_node : public visitable_node<other_mock_node> {
public:
    other_mock_node()
        : visitable_node<other_mock_node>(diagnostic_context_location())
    {
        return;
    }
};

class mock_node_visitor {
public:
    void visit(mock_node &) const
    {
        LOG_INFO("visited mock_node");
    }

    void visit(other_mock_node &) const
    {
        LOG_INFO("visited other_mock_node");
    }
};

begin_suite(node_test);

test_case(simple_visit)
{
    mock_node mn;
    other_mock_node omn;

    mock_node *mnp = &mn;
    other_mock_node *omnp = &omn;

    assert_log_empty();
    visit(mock_node_visitor(), mnp);
    assert_log_message(log_level::info, "<BUFFER>: visited mock_node");
    assert_log_empty();

    visit(mock_node_visitor(), omnp);
    assert_log_message(log_level::info, "<BUFFER>: visited other_mock_node");
    assert_log_empty();
}

test_case(variant_visit)
{
    mock_node mn;
    other_mock_node omn;

    variant<mock_node*, other_mock_node*> mnp(&mn);
    variant<mock_node*, other_mock_node*> omnp(&omn);

    assert_log_empty();
    visit(mock_node_visitor(), mnp);
    assert_log_message(log_level::info, "<BUFFER>: visited mock_node");
    assert_log_empty();

    visit(mock_node_visitor(), omnp);
    assert_log_message(log_level::info, "<BUFFER>: visited other_mock_node");
    assert_log_empty();
}

end_suite(node_test);
