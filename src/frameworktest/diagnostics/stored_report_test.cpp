#include <nullunit/nullunit.h>
#include <sstream>

#include "framework/diagnostics/stored_report.h"

using namespace Gorc::Diagnostics;

class StoredReportTestFixture : public NullUnit::Fixture {
public:
	StoredReport report;
};

BeginSuiteFixture(StoredReportTest, StoredReportTestFixture);

Case(AddError) {
	Test_Assert_Eq(report.GetWarningCount(), 0);
	Test_Assert_Eq(report.GetErrorCount(), 0);

	report.AddError("Stage 0", "Reason", ErrorLocation("test.w", 15, 16, 10, 11));

	Test_Assert_Eq(report.GetWarningCount(), 0);
	Test_Assert_Eq(report.GetErrorCount(), 1);

	auto it = report.begin();
	Test_Assert_Eq(it->Level, ErrorLevel::Error);
	Test_Assert_Eq(it->Stage, "Stage 0");
	Test_Assert_Eq(it->Reason, "Reason");

	ErrorLocation l("test.w", 15, 16, 10, 11);

	Test_Assert_Eq(it->Location, l);
}

Case(AddWarning) {
	Test_Assert_Eq(report.GetErrorCount(), 0);
	Test_Assert_Eq(report.GetWarningCount(), 0);

	report.AddWarning("Stage 0", "Reason", ErrorLocation("test.w", 15, 16, 10, 11));

	Test_Assert_Eq(report.GetErrorCount(), 0);
	Test_Assert_Eq(report.GetWarningCount(), 1);

	auto it = report.begin();
	Test_Assert_Eq(it->Level, ErrorLevel::Warning);
	Test_Assert_Eq(it->Stage, "Stage 0");
	Test_Assert_Eq(it->Reason, "Reason");

	ErrorLocation l("test.w", 15, 16, 10, 11);

	Test_Assert_Eq(it->Location, l);
}

Case(AddCriticalError) {
	Test_Assert_Eq(report.GetWarningCount(), 0);
	Test_Assert_Eq(report.GetErrorCount(), 0);

	report.AddCriticalError("Stage 0", "Reason");

	Test_Assert_Eq(report.GetWarningCount(), 0);
	Test_Assert_Eq(report.GetErrorCount(), 1);

	auto it = report.begin();
	Test_Assert_Eq(it->Level, ErrorLevel::CriticalError);
	Test_Assert_Eq(it->Stage, "Stage 0");
	Test_Assert_Eq(it->Reason, "Reason");
}

EndSuite(StoredReportTest);
