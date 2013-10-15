#include "stored_report.h"
#include "error_level.h"

using namespace gorc::diagnostics;

stored_report::stored_report()
	: errorCount(0), warningCount(0) {
	return;
}

void stored_report::add_error(const std::string& stage, const std::string& reason, const error_location& errorLocation) {
	container.push_back(error(error_level::error, stage, reason, errorLocation));
	++errorCount;
}

void stored_report::add_warning(const std::string& stage, const std::string& reason, const error_location& errorLocation) {
	container.push_back(error(error_level::warning, stage, reason, errorLocation));
	++warningCount;
}

void stored_report::add_critical_error(const std::string& stage, const std::string& reason) {
	container.push_back(error(error_level::critical_error, stage, reason, error_location()));
	++errorCount;
}

unsigned int stored_report::get_error_count() const {
	return errorCount;
}

unsigned int stored_report::get_warning_count() const {
	return warningCount;
}

void stored_report::repeat(report& report) const {
	for(const auto& err : container) {
		switch(err.level) {
		case error_level::critical_error:
			report.add_critical_error(err.stage, err.reason);
			break;

		case error_level::error:
			report.add_error(err.stage, err.reason, err.location);
			break;

		case error_level::warning:
			report.add_warning(err.stage, err.reason, err.location);
			break;
		}
	}

	return;
}
