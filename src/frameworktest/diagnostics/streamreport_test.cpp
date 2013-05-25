#include <nullunit/nullunit.h>
#include <sstream>

#include "framework/diagnostics/streamreport.h"

using namespace Gorc::Diagnostics;

class StreamReportTestFixture : public NullUnit::Fixture {
public:
	std::ostringstream dummystream;
	StreamReport report;

	StreamReportTestFixture()
		: report(dummystream) {
		return;
	}
};

BeginSuiteFixture(StreamReportTest, StreamReportTestFixture);

Case(AddError) {
	Test_Assert_Eq(report.GetWarningCount(), 0);
	Test_Assert_Eq(report.GetErrorCount(), 0);

	report.AddError("Stage 0", "Reason", ErrorLocation("test.w", 15, 16, 10, 11));

	Test_Assert_Eq(report.GetWarningCount(), 0);
	Test_Assert_Eq(report.GetErrorCount(), 1);
}

Case(AddWarning) {
	Test_Assert_Eq(report.GetErrorCount(), 0);
	Test_Assert_Eq(report.GetWarningCount(), 0);

	report.AddWarning("Stage 0", "Reason", ErrorLocation("test.w", 15, 16, 10, 11));

	Test_Assert_Eq(report.GetErrorCount(), 0);
	Test_Assert_Eq(report.GetWarningCount(), 1);
}

Case(AddCriticalError) {
	Test_Assert_Eq(report.GetWarningCount(), 0);
	Test_Assert_Eq(report.GetErrorCount(), 0);

	report.AddCriticalError("Stage 0", "Reason");

	Test_Assert_Eq(report.GetWarningCount(), 0);
	Test_Assert_Eq(report.GetErrorCount(), 1);
}

EndSuite(StreamReportTest);
