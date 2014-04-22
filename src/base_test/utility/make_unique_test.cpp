#include "test/test.h"
#include "base/utility/make_unique.h"

int test_adt_constructor_count;
int test_adt_destructor_count;
int test_adt_constructor_arg_sum;
int test_adt_value;

class test_adt {
public:
    test_adt() {
        ++test_adt_constructor_count;
    }

    test_adt(int a) {
        ++test_adt_constructor_count;
        test_adt_constructor_arg_sum = a;
    }

    test_adt(int a, int b, int c) {
        ++test_adt_constructor_count;
        test_adt_constructor_arg_sum = a + b + c;
    }

    ~test_adt() {
        ++test_adt_destructor_count;
    }

    void set_value(int v) {
        test_adt_value = v;
    }
};

class make_unique_test_fixture : public test::fixture {
public:
    virtual void setup() override {
        test_adt_constructor_count = 0;
        test_adt_destructor_count = 0;
        test_adt_constructor_arg_sum = 0;
        test_adt_value = 0;
    }
};

begin_suite_fixture(make_unique_test, make_unique_test_fixture);

test_case(default_ctor) {
    assert_eq(test_adt_constructor_count, 0);
    assert_eq(test_adt_destructor_count, 0);
    assert_eq(test_adt_constructor_arg_sum, 0);

    do {
        auto adt = gorc::make_unique<test_adt>();

        assert_eq(test_adt_constructor_count, 1);
        assert_eq(test_adt_destructor_count, 0);
        assert_eq(test_adt_constructor_arg_sum, 0);

        adt->set_value(5);
        assert_eq(test_adt_value, 5);
    } while(false);

    assert_eq(test_adt_constructor_count, 1);
    assert_eq(test_adt_destructor_count, 1);
}

test_case(sv_ctor) {
    assert_eq(test_adt_constructor_count, 0);
    assert_eq(test_adt_destructor_count, 0);
    assert_eq(test_adt_constructor_arg_sum, 0);

    do {
        auto adt = gorc::make_unique<test_adt>(8);

        assert_eq(test_adt_constructor_count, 1);
        assert_eq(test_adt_destructor_count, 0);
        assert_eq(test_adt_constructor_arg_sum, 8);

        adt->set_value(5);
        assert_eq(test_adt_value, 5);
    } while(false);

    assert_eq(test_adt_constructor_count, 1);
    assert_eq(test_adt_destructor_count, 1);
}

test_case(mv_ctor) {
    assert_eq(test_adt_constructor_count, 0);
    assert_eq(test_adt_destructor_count, 0);
    assert_eq(test_adt_constructor_arg_sum, 0);

    do {
        auto adt = gorc::make_unique<test_adt>(1, 2, 3);

        assert_eq(test_adt_constructor_count, 1);
        assert_eq(test_adt_destructor_count, 0);
        assert_eq(test_adt_constructor_arg_sum, 6);

        adt->set_value(5);
        assert_eq(test_adt_value, 5);
    } while(false);

    assert_eq(test_adt_constructor_count, 1);
    assert_eq(test_adt_destructor_count, 1);
}

end_suite(make_unique_test);
