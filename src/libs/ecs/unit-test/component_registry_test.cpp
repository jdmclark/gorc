#include "test/test.hpp"
#include "ecs/component_registry.hpp"
#include "utility/uid.hpp"
#include "log/log.hpp"

namespace {
    class component_a {
    public:
        uid(589475);
    };

    class component_b {
    public:
        uid(584755);
    };
}

begin_suite(component_registry_test);

test_case(simple_register)
{
    gorc::component_registry<gorc::thing_id> compreg;

    compreg.register_component_type<component_a>();
    compreg.register_component_type<component_b>();

    assert_log_empty();
}

test_case(conflict)
{
    gorc::component_registry<gorc::thing_id> compreg;

    compreg.register_component_type<component_a>();

    assert_log_empty();

    assert_throws_logged(compreg.register_component_type<component_a>());

    assert_log_message(gorc::log_level::error,
                       "component type with uid 589475 is already registered");
    assert_log_empty();
}

end_suite(component_registry_test);
