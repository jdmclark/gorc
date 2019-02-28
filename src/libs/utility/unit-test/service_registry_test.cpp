#include "test/test.hpp"
#include "utility/service_registry.hpp"

begin_suite(service_registry_test);

test_case(register_set_get)
{
    std::string msg = "Hello, World!";

    gorc::service_registry sr;
    sr.add(msg);

    auto &reg_msg = sr.get<std::string>();

    assert_eq(reg_msg, std::string("Hello, World!"));
}

test_case(register_reregister)
{
    std::string msg = "Hello, World!";

    gorc::service_registry sr;
    sr.add(msg);

    assert_throws_type(sr.add(msg), std::runtime_error);
}

test_case(register_get_unregistered)
{
    gorc::service_registry sr;

    assert_throws_type(sr.get<std::string>(), std::runtime_error);
}

test_case(add_or_replace)
{
    gorc::service_registry sr;

    std::string msg1 = "Hello, World!";

    sr.add_or_replace(msg1);
    auto &reg_msg1 = sr.get<std::string>();

    assert_eq(reg_msg1, std::string("Hello, World!"));

    std::string msg2 = "foobarbaz";
    sr.add_or_replace(msg2);

    auto &reg_msg2 = sr.get<std::string>();

    assert_eq(reg_msg2, std::string("foobarbaz"));
}

test_case(parent)
{
    gorc::service_registry parent;

    std::string msg1 = "Hello, World!";
    parent.add_or_replace(msg1);

    assert_eq(parent.get<std::string>(), std::string("Hello, World!"));

    gorc::service_registry child(&parent);

    assert_eq(child.get<std::string>(), std::string("Hello, World!"));

    std::string msg2 = "Goodbye, Microcosm!";
    child.add_or_replace(msg2);

    assert_eq(child.get<std::string>(), std::string("Goodbye, Microcosm!"));

    assert_eq(parent.get<std::string>(), std::string("Hello, World!"));
}

test_case(has)
{
    gorc::service_registry parent;
    gorc::service_registry child(&parent);

    assert_true(!parent.has<std::string>());
    assert_true(!child.has<std::string>());

    std::string msg1 = "Hello, World!";
    parent.add_or_replace(msg1);
    assert_true(parent.has<std::string>());
    assert_true(child.has<std::string>());

    int msg2 = 3;
    child.add_or_replace(msg2);
    assert_true(!parent.has<int>());
    assert_true(child.has<int>());
}

end_suite(service_registry_test);
