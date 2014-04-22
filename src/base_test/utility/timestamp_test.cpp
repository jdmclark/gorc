#include "test/test.h"
#include "base/utility/timestamp.h"

begin_suite(timestamp_test);

test_case(as_ms_test) {
    gorc::timestamp ts(1000);
    assert_eq(ts.as_milliseconds(), 1000U);
}

end_suite(timestamp_test);
