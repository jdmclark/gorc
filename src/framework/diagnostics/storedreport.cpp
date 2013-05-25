#include "storedreport.h"
#include "errorlevel.h"

using namespace Gorc::Diagnostics;

StoredReport::StoredReport()
	: errorCount(0), warningCount(0) {
	return;
}

void StoredReport::AddError(const std::string& stage, const std::string& reason, const ErrorLocation& errorLocation) {
	container.push_back(Error(ErrorLevel::Error, stage, reason, errorLocation));
	++errorCount;
}

void StoredReport::AddWarning(const std::string& stage, const std::string& reason, const ErrorLocation& errorLocation) {
	container.push_back(Error(ErrorLevel::Warning, stage, reason, errorLocation));
	++warningCount;
}

void StoredReport::AddCriticalError(const std::string& stage, const std::string& reason) {
	container.push_back(Error(ErrorLevel::CriticalError, stage, reason, ErrorLocation()));
	++errorCount;
}

unsigned int StoredReport::GetErrorCount() const {
	return errorCount;
}

unsigned int StoredReport::GetWarningCount() const {
	return warningCount;
}

void StoredReport::Repeat(Report& report) const {
	for(const auto& err : container) {
		switch(err.Level) {
		case ErrorLevel::CriticalError:
			report.AddCriticalError(err.Stage, err.Reason);
			break;

		case ErrorLevel::Error:
			report.AddError(err.Stage, err.Reason, err.Location);
			break;

		case ErrorLevel::Warning:
			report.AddWarning(err.Stage, err.Reason, err.Location);
			break;
		}
	}

	return;
}
