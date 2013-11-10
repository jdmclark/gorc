#include <nullunit/nullunit.h>
#include <sstream>

#include "framework/diagnostics/stream_report.h"

using namespace gorc::diagnostics;

class StreamReportTestFixture : public NullUnit::Fixture {
public:
	std::ostringstream dummystream;
	stream_report report;

	StreamReportTestFixture()
		: report(dummystream) {
		return;
	}
};

BeginSuiteFixture(StreamReportTest, StreamReportTestFixture);

Case(add_error) {
	Test_Assert_Eq(report.get_warning_count(), 0);
	Test_Assert_Eq(report.get_error_count(), 0);

	report.add_error("Stage 0", "Reason", error_location("test.w", 15, 16, 10, 11));

	Test_Assert_Eq(report.get_warning_count(), 0);
	Test_Assert_Eq(report.get_error_count(), 1);
}

Case(add_warning) {
	Test_Assert_Eq(report.get_error_count(), 0);
	Test_Assert_Eq(report.get_warning_count(), 0);

	report.add_warning("Stage 0", "Reason", error_location("test.w", 15, 16, 10, 11));

	Test_Assert_Eq(report.get_error_count(), 0);
	Test_Assert_Eq(report.get_warning_count(), 1);
}

Case(add_critical_error) {
	Test_Assert_Eq(report.get_warning_count(), 0);
	Test_Assert_Eq(report.get_error_count(), 0);

	report.add_critical_error("Stage 0", "Reason");

	Test_Assert_Eq(report.get_warning_count(), 0);
	Test_Assert_Eq(report.get_error_count(), 1);
}

EndSuite(StreamReportTest);
