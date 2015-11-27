#include "test/test.hpp"
#include "ast/factory.hpp"
#include "log/log.hpp"

class mock_node {
public:
    int value = 0;

    mock_node(int v)
        : value(v)
    {
        LOG_INFO("mock_node constructor");
    }

    ~mock_node()
    {
        LOG_INFO("mock_node destructor");
    }
};

begin_suite(ast_factory_test);

test_case(destructors_called)
{
    auto fac = gorc::make_unique<gorc::ast_factory>();

    assert_log_empty();

    auto node = fac->make<mock_node>(5);

    assert_log_message(gorc::log_level::info, "mock_node constructor");
    assert_log_empty();

    assert_eq(node->value, 5);

    fac.reset();

    assert_log_message(gorc::log_level::info, "mock_node destructor");
    assert_log_empty();
}

end_suite(ast_factory_test);
