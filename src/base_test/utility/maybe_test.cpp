#include "test/test.h"
#include "base/utility/maybe.h"

using namespace gorc;

begin_suite(maybe_test);

test_case(default_has_no_value) {
    maybe<int> m;
    if(int* i = m) {
        assert_always("entered if statement");
    }

    assert_eq(m.has_value(), false);
}

test_case(initialize_with_value) {
    maybe<int> m(5);
    if(int* i = m) {
        assert_eq(*i, 5);
    }
    else {
        assert_always("did not enter if statement");
    }

    assert_true(m.has_value());
}

test_case(assign_with_value) {
    maybe<int> m;
    assert_eq(m.has_value(), false);

    m = 5;

    assert_eq(m.has_value(), true);
    if(int* i = m) {
        assert_eq(*i, 5);
    }
    else {
        assert_always("did not enter if statement");
    }
}

test_case(copy_ctor) {
    maybe<int> m(5);

    maybe<int> n(m);

    assert_eq(n.has_value(), true);
    if(int* i = n) {
        assert_eq(*i, 5);
    }
    else {
        assert_always("did not enter if statement");
    }
}

test_case(copy_assign) {
    maybe<int> m(5);

    maybe<int> n(17);

    n = m;

    assert_eq(n.has_value(), true);
    if(int* i = n) {
        assert_eq(*i, 5);
    }
    else {
        assert_always("did not enter if statement");
    }
}

test_case(pointer_default_has_no_value) {
    maybe<int*> m;
    if(int* i = m) {
        assert_always("entered if statement");
    }

    assert_eq(m.has_value(), false);
}

test_case(pointer_initialize_with_value) {
    int v = 5;
    maybe<int*> m(&v);
    if(int* i = m) {
        assert_eq(*i, 5);
        assert_eq(i, &v);
    }
    else {
        assert_always("did not enter if statement");
    }

    assert_true(m.has_value());
}

test_case(pointer_assign_with_value) {
    int v = 5;
    maybe<int*> m;
    assert_eq(m.has_value(), false);

    m = &v;

    assert_eq(m.has_value(), true);
    if(int* i = m) {
        assert_eq(*i, 5);
        assert_eq(i, &v);
    }
    else {
        assert_always("did not enter if statement");
    }
}

test_case(pointer_copy_ctor) {
    int v = 5;
    maybe<int*> m(&v);

    maybe<int*> n(m);

    assert_eq(n.has_value(), true);
    if(int* i = n) {
        assert_eq(*i, 5);
        assert_eq(i, &v);
    }
    else {
        assert_always("did not enter if statement");
    }
}

test_case(pointer_copy_assign) {
    int v = 5;
    maybe<int*> m(&v);

    int w = 17;
    maybe<int*> n(&w);

    n = m;

    assert_eq(n.has_value(), true);
    if(int* i = n) {
        assert_eq(*i, 5);
        assert_eq(i, &v);
    }
    else {
        assert_always("did not enter if statement");
    }
}

end_suite(maybe_test);
