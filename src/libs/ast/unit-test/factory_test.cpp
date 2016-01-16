#include "test/test.hpp"
#include "ast/factory.hpp"
#include "log/log.hpp"

namespace {

    class factory_mock_node {
    public:
        int value = 0;

        factory_mock_node(int v)
            : value(v)
        {
            LOG_INFO("factory_mock_node constructor");
        }

        ~factory_mock_node()
        {
            LOG_INFO("factory_mock_node destructor");
        }
    };

}

begin_suite(ast_factory_test);

test_case(destructors_called)
{
    auto fac = std::make_unique<gorc::ast_factory>();

    assert_log_empty();

    auto node = fac->make<factory_mock_node>(5);

    assert_log_message(gorc::log_level::info, "factory_mock_node constructor");
    assert_log_empty();

    assert_eq(node->value, 5);

    fac.reset();

    assert_log_message(gorc::log_level::info, "factory_mock_node destructor");
    assert_log_empty();
}

end_suite(ast_factory_test);
