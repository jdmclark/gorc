#include "test/test.hpp"
#include "system/env.hpp"

begin_suite(env_test);

test_case(has_env_var)
{
    assert_true(!gorc::has_environment_variable("TEST_ENV_VAR_UNSET_01"));
    auto nv = gorc::get_environment_variable("TEST_ENV_VAR_UNSET_01");
    assert_true(!nv.has_value());

    gorc::set_environment_variable("TEST_ENV_VAR_UNSET_01", "1");
    assert_true(gorc::has_environment_variable("TEST_ENV_VAR_UNSET_01"));

    auto ev = gorc::get_environment_variable("TEST_ENV_VAR_UNSET_01");
    assert_true(ev.has_value());
    assert_eq(ev.get_value(), std::string("1"));

    gorc::set_environment_variable("TEST_ENV_VAR_UNSET_01", "");
    assert_true(!gorc::has_environment_variable("TEST_ENV_VAR_UNSET_01"));

    auto mv = gorc::get_environment_variable("TEST_ENV_VAR_UNSET_01");
    assert_true(!mv.has_value());
}

end_suite(env_test);
