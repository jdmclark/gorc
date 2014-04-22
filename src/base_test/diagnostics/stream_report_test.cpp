#include "test/test.h"
#include <sstream>

#include "base/diagnostics/stream_report.h"

using namespace gorc::diagnostics;

class stream_report_test_fixture : public test::fixture {
public:
    std::ostringstream dummystream;
    stream_report report;

    stream_report_test_fixture()
        : report(dummystream) {
        return;
    }
};

begin_suite_fixture(stream_report_test, stream_report_test_fixture);

test_case(add_error) {
    assert_eq(report.get_warning_count(), 0UL);
    assert_eq(report.get_error_count(), 0UL);

    report.add_error("Stage 0", "Reason", error_location("test.w", 15, 16, 10, 11));

    assert_eq(report.get_warning_count(), 0UL);
    assert_eq(report.get_error_count(), 1UL);
}

test_case(add_warning) {
    assert_eq(report.get_error_count(), 0UL);
    assert_eq(report.get_warning_count(), 0UL);

    report.add_warning("Stage 0", "Reason", error_location("test.w", 15, 16, 10, 11));

    assert_eq(report.get_error_count(), 0UL);
    assert_eq(report.get_warning_count(), 1UL);
}

test_case(add_critical_error) {
    assert_eq(report.get_warning_count(), 0UL);
    assert_eq(report.get_error_count(), 0UL);

    report.add_critical_error("Stage 0", "Reason");

    assert_eq(report.get_warning_count(), 0UL);
    assert_eq(report.get_error_count(), 1UL);
}

end_suite(stream_report_test);
