#include "test/test.hpp"
#include "jk/cog/script/verb_traits.hpp"
#include "utility/service_registry.hpp"

using namespace gorc;
using namespace gorc::cog;

begin_suite(verb_traits_test);

test_case(verb_arity_zero)
{
    auto fn = [] { };
    assert_eq(compute_verb_arity<decltype(fn)>(), size_t(0));
}

test_case(verb_arity_one)
{
    auto fn = [] (int) { };
    assert_eq(compute_verb_arity<decltype(fn)>(), size_t(1));
}

test_case(void_result)
{
    auto fn = [] { };
    assert_eq(compute_verb_result_type(fn), value_type::nothing);
}

test_case(multi_argument_sector_result)
{
    auto fn = [](int, int, int) { return sector_id(5); };
    assert_eq(compute_verb_result_type(fn), value_type::sector);
}

test_case(multi_argument_type)
{
    auto fn = [](int, float, char const *, vector<3>) { return; };
    assert_eq(compute_verb_result_type(fn), value_type::nothing);

    assert_eq(compute_verb_argument_type<0>(fn), value_type::floating);
    assert_eq(compute_verb_argument_type<1>(fn), value_type::floating);
    assert_eq(compute_verb_argument_type<2>(fn), value_type::string);
    assert_eq(compute_verb_argument_type<3>(fn), value_type::vector);
}

test_case(multi_argument_store)
{
    auto fn = [](int, float, char const *, vector<3>, sector_id) { return; };

    std::vector<value_type> v;
    compute_verb_argument_types(fn, std::back_inserter(v));

    assert_eq(v, (std::vector<value_type> { value_type::floating,
                                            value_type::floating,
                                            value_type::string,
                                            value_type::vector,
                                            value_type::sector }));
}

test_case(service_verb_arity)
{
    auto fn = [](service_registry&, int, int, int) { return; };

    assert_eq(compute_verb_arity<decltype(fn)>(), size_t(4));
}

end_suite(verb_traits_test);
