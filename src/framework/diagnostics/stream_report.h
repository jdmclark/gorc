#pragma once

#include "report.h"
#include <iosfwd>

namespace gorc {
namespace diagnostics {

class stream_report : public report {
private:
	unsigned int errorCount, warningCount;
	std::ostream& stream;

	void PrintFormattedError(const std::string& errorLevel, const std::string& stage, const std::string& reason);
	void PrintLineFormattedError(const std::string& errorLevel, const std::string& stage, const std::string& reason, const error_location& location);

public:
	stream_report(std::ostream& stream);

	void add_error(const std::string& stage, const std::string& reason, const error_location& location);
	void add_warning(const std::string& stage, const std::string& reason, const error_location& location);
	void add_critical_error(const std::string& stage, const std::string& reason);

	unsigned int get_error_count() const;
	unsigned int get_warning_count() const;
};

}
}
