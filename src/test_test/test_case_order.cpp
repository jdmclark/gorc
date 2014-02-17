#include "test/test.h"

namespace test_test {

begin_suite(case_order_suite);

test_case(case1) { }

test_case(validate_case_order) {
    test::suite_map& factories = test::suite_registry::get().factories;

    auto it = factories.find("case_order_suite");
    assert_ne(it, factories.end());

    auto suite = it->second->create_suite();

    test::case_vector& cases = suite->get_case_factories();

    assert_eq(cases[0].first, "case1");
    assert_eq(cases[1].first, "validate_case_order");
    assert_eq(cases[2].first, "case3");
    assert_eq(cases[3].first, "case5");
    assert_eq(cases[4].first, "case2");
    assert_eq(cases[5].first, "case6");
}

test_case(case3) { }

test_case(case5) { }

test_case(case2) { }

test_case(case6) { }

end_suite(case_order_suite);

}
