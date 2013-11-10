#include "stream_report.h"

#include <iomanip>
#include <iostream>

using namespace gorc::diagnostics;

stream_report::stream_report(std::ostream& s)
	: errorCount(0), warningCount(0), stream(s) {
	return;
}

void stream_report::PrintFormattedError(const std::string& level, const std::string& stage, const std::string& reason) {
	stream << level << ": " << reason << " [" << stage << "]" << std::endl;
}

void stream_report::PrintLineFormattedError(const std::string& level, const std::string& stage, const std::string& reason, const error_location& location) {
	stream << location.filename << "(" << location.first_line << "," << location.first_column
			<< "): " << level << ": " << reason << " [" << stage << "]" << std::endl;
}

void stream_report::add_error(const std::string& stage, const std::string& reason, const error_location& location) {
	PrintLineFormattedError("error", stage, reason, location);
	++errorCount;
}

void stream_report::add_warning(const std::string& stage, const std::string& reason, const error_location& location) {
	PrintLineFormattedError("warning", stage, reason, location);
	++warningCount;
}

void stream_report::add_critical_error(const std::string& stage, const std::string& reason) {
	PrintFormattedError("critical error", stage, reason);
	++errorCount;
}

unsigned int stream_report::get_error_count() const {
	return errorCount;
}

unsigned int stream_report::get_warning_count() const {
	return warningCount;
}
