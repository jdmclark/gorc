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

end_suite(service_registry_test);
