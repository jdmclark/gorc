#include <nullunit/nullunit.h>
#include <sstream>

#include "framework/diagnostics/stored_report.h"

using namespace gorc::diagnostics;

class StoredReportTestFixture : public NullUnit::Fixture {
public:
	stored_report report;
};

BeginSuiteFixture(StoredReportTest, StoredReportTestFixture);

Case(add_error) {
	Test_Assert_Eq(report.get_warning_count(), 0);
	Test_Assert_Eq(report.get_error_count(), 0);

	report.add_error("Stage 0", "Reason", error_location("test.w", 15, 16, 10, 11));

	Test_Assert_Eq(report.get_warning_count(), 0);
	Test_Assert_Eq(report.get_error_count(), 1);

	auto it = report.begin();
	Test_Assert_Eq(it->level, error_level::error);
	Test_Assert_Eq(it->stage, "Stage 0");
	Test_Assert_Eq(it->reason, "Reason");

	error_location l("test.w", 15, 16, 10, 11);

	Test_Assert_Eq(it->location, l);
}

Case(add_warning) {
	Test_Assert_Eq(report.get_error_count(), 0);
	Test_Assert_Eq(report.get_warning_count(), 0);

	report.add_warning("Stage 0", "Reason", error_location("test.w", 15, 16, 10, 11));

	Test_Assert_Eq(report.get_error_count(), 0);
	Test_Assert_Eq(report.get_warning_count(), 1);

	auto it = report.begin();
	Test_Assert_Eq(it->level, error_level::warning);
	Test_Assert_Eq(it->stage, "Stage 0");
	Test_Assert_Eq(it->reason, "Reason");

	error_location l("test.w", 15, 16, 10, 11);

	Test_Assert_Eq(it->location, l);
}

Case(add_critical_error) {
	Test_Assert_Eq(report.get_warning_count(), 0);
	Test_Assert_Eq(report.get_error_count(), 0);

	report.add_critical_error("Stage 0", "Reason");

	Test_Assert_Eq(report.get_warning_count(), 0);
	Test_Assert_Eq(report.get_error_count(), 1);

	auto it = report.begin();
	Test_Assert_Eq(it->level, error_level::critical_error);
	Test_Assert_Eq(it->stage, "Stage 0");
	Test_Assert_Eq(it->reason, "Reason");
}

EndSuite(StoredReportTest);
