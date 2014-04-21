#include "test/test.h"

begin_suite(failure_suite);

test_case(pass) {
}

test_case(valued_print_fail) {
    int v = 7;
    int h = 2;
    assert_eq(7 + 2 * 3, v * 2);
}

test_case(non_valued_print_fail) {
    struct foo {
    public:
        bool operator==(const foo&) const {
            return false;
        }
    };

    foo a, b;

    assert_eq(a, b);
}

test_case(assertion_failure) {
    assert_always("always failing in assertion");
}

test_case(expectation_failure) {
    expect_always("always failing in expectation");
}

test_case(unhandled_std_exception_failure) {
    throw std::exception();
}

test_case(unhandled_unknown_exception_failure) {
    class unknown_exception {
    public:
        unknown_exception() {
            return;
        }
    };

    throw unknown_exception();
}

end_suite(failure_suite);

class setup_failure_fixture : public test::fixture {
public:
    virtual void setup() override {
        throw std::exception();
    }
};

begin_suite_fixture(setup_failure, setup_failure_fixture);

test_case(always_passes) {
}

end_suite(setup_failure);

class teardown_failure_fixture : public test::fixture {
public:
    virtual void teardown() override {
        throw std::exception();
    }
};

begin_suite_fixture(teardown_failure, teardown_failure_fixture);

test_case(always_passes) {
}

end_suite(teardown_failure);
