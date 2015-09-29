#include "cog/script/function_verb.hpp"
#include "test/test.hpp"

using namespace gorc;
using namespace gorc::cog;

begin_suite(function_verb_test);

test_case(function_verb_no_args)
{
    auto fn = []()-> sector_id { return sector_id(5); };
    auto vn = make_function_verb("myverb", fn);

    // Test that traits are correct
    assert_eq(vn->name, std::string("myverb"));
    assert_eq(vn->return_type, value_type::sector);
    assert_true(vn->argument_types.empty());

    // Test call
    cog::stack stk;
    service_registry sr;
    auto rv = vn->invoke(stk, sr);

    sector_id rvid = rv;
    assert_eq(rvid, sector_id(5));
}

test_case(function_verb_arg_order)
{
    auto fn = [](int a, int b, int c)
    {
        LOG_INFO(format("%d %d %d") % a % b % c);
    };

    auto vn = make_function_verb("myverb", fn);

    assert_eq(vn->argument_types.size(), size_t(3));

    cog::stack stk;
    service_registry sr;

    // Push arguments left to right
    stk.push(5);
    stk.push(10);
    stk.push(3);

    auto rv = vn->invoke(stk, sr);
    assert_eq(rv.get_type(), value_type::nothing);

    assert_log_message(log_level::info, "5 10 3");
    assert_log_empty();

    assert_true(stk.empty());
}

test_case(function_verb_underflow)
{
    auto fn = [](int a, int b, int c)
    {
        LOG_INFO(format("%d %d %d") % a % b % c);
    };

    auto vn = make_function_verb("myverb", fn);

    cog::stack stk;
    service_registry sr;

    // Push arguments left to right
    stk.push(5);
    stk.push(10);

    assert_throws_logged(vn->invoke(stk, sr));

    assert_log_message(log_level::error, "stack underflow in verb 'myverb'");
    assert_log_empty();
}

end_suite(function_verb_test);
