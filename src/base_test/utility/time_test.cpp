#include "test/test.h"
#include "base/utility/time.h"

begin_suite(time_test);

test_case(interface) {
    gorc::timestamp current(2500);
    gorc::timestamp prev(1500);

    gorc::time t(current, prev);

    assert_eq(t.timestamp(), current);
    assert_ge(t.elapsed_as_seconds(), 1.0);
    assert_le(t.elapsed_as_seconds(), 1.0);
}

end_suite(time_test);
