#pragma once

#include "report.h"
#include <iosfwd>

namespace Gorc {
namespace Diagnostics {

class StreamReport : public Report {
private:
	unsigned int errorCount, warningCount;
	std::ostream& stream;

	void PrintFormattedError(const std::string& errorLevel, const std::string& stage, const std::string& reason);
	void PrintLineFormattedError(const std::string& errorLevel, const std::string& stage, const std::string& reason, const ErrorLocation& location);

public:
	StreamReport(std::ostream& stream);

	void AddError(const std::string& stage, const std::string& reason, const ErrorLocation& location);
	void AddWarning(const std::string& stage, const std::string& reason, const ErrorLocation& location);
	void AddCriticalError(const std::string& stage, const std::string& reason);

	unsigned int GetErrorCount() const;
	unsigned int GetWarningCount() const;
};

}
}
