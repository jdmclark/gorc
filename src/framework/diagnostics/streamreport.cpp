#include "streamreport.h"

#include <iomanip>
#include <iostream>

using namespace Gorc::Diagnostics;

StreamReport::StreamReport(std::ostream& s)
	: errorCount(0), warningCount(0), stream(s) {
	return;
}

void StreamReport::PrintFormattedError(const std::string& level, const std::string& stage, const std::string& reason) {
	stream << level << ": " << reason << " [" << stage << "]" << std::endl;
}

void StreamReport::PrintLineFormattedError(const std::string& level, const std::string& stage, const std::string& reason, const ErrorLocation& location) {
	stream << location.filename << "(" << location.first_line << "," << location.first_column
			<< "): " << level << ": " << reason << " [" << stage << "]" << std::endl;
}

void StreamReport::AddError(const std::string& stage, const std::string& reason, const ErrorLocation& location) {
	PrintLineFormattedError("error", stage, reason, location);
	++errorCount;
}

void StreamReport::AddWarning(const std::string& stage, const std::string& reason, const ErrorLocation& location) {
	PrintLineFormattedError("warning", stage, reason, location);
	++warningCount;
}

void StreamReport::AddCriticalError(const std::string& stage, const std::string& reason) {
	PrintFormattedError("critical error", stage, reason);
	++errorCount;
}

unsigned int StreamReport::GetErrorCount() const {
	return errorCount;
}

unsigned int StreamReport::GetWarningCount() const {
	return warningCount;
}
