#include "test/test.h"
#include "base/utility/time_span.h"

begin_suite(time_span_test);

test_case(diff_as_secs) {
    gorc::time_span ts(gorc::timestamp(1500), gorc::timestamp(2500));
    assert_ge(ts.as_seconds(), 1.0);
    assert_le(ts.as_seconds(), 1.0);
}

test_case(sub_as_secs) {
    auto ts = gorc::timestamp(2500) - gorc::timestamp(1500);
    assert_ge(ts.as_seconds(), 1.0);
    assert_le(ts.as_seconds(), 1.0);
}

test_case(sub_as_secs_negative) {
    auto ts = gorc::timestamp(1500) - gorc::timestamp(2500);
    assert_ge(ts.as_seconds(), -1.0);
    assert_le(ts.as_seconds(), -1.0);
}

end_suite(time_span_test);
