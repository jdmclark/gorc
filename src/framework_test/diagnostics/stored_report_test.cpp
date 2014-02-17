#include "test/test.h"
#include <sstream>

#include "framework/diagnostics/stored_report.h"

using namespace gorc::diagnostics;

class stored_report_test_fixture : public test::fixture {
public:
    stored_report report;
};

begin_suite_fixture(stored_report_test, stored_report_test_fixture);

test_case(add_error) {
    assert_eq(report.get_warning_count(), 0);
    assert_eq(report.get_error_count(), 0);

    report.add_error("Stage 0", "Reason", error_location("test.w", 15, 16, 10, 11));

    assert_eq(report.get_warning_count(), 0);
    assert_eq(report.get_error_count(), 1);

    auto it = report.begin();
    assert_eq(it->level, error_level::error);
    assert_eq(it->stage, "Stage 0");
    assert_eq(it->reason, "Reason");

    error_location l("test.w", 15, 16, 10, 11);

    assert_eq(it->location, l);
}

test_case(add_warning) {
    assert_eq(report.get_error_count(), 0);
    assert_eq(report.get_warning_count(), 0);

    report.add_warning("Stage 0", "Reason", error_location("test.w", 15, 16, 10, 11));

    assert_eq(report.get_error_count(), 0);
    assert_eq(report.get_warning_count(), 1);

    auto it = report.begin();
    assert_eq(it->level, error_level::warning);
    assert_eq(it->stage, "Stage 0");
    assert_eq(it->reason, "Reason");

    error_location l("test.w", 15, 16, 10, 11);

    assert_eq(it->location, l);
}

test_case(add_critical_error) {
    assert_eq(report.get_warning_count(), 0);
    assert_eq(report.get_error_count(), 0);

    report.add_critical_error("Stage 0", "Reason");

    assert_eq(report.get_warning_count(), 0);
    assert_eq(report.get_error_count(), 1);

    auto it = report.begin();
    assert_eq(it->level, error_level::critical_error);
    assert_eq(it->stage, "Stage 0");
    assert_eq(it->reason, "Reason");
}

end_suite(stored_report_test);
