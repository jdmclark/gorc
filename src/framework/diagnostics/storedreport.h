#pragma once

#include "report.h"
#include "error.h"

#include <vector>

namespace Gorc {
namespace Diagnostics {

class StoredReport : public Report {
private:
	std::vector<Error> container;
	unsigned int errorCount, warningCount;

public:
	StoredReport();

	void AddError(const std::string& stage, const std::string& reason, const ErrorLocation& location);
	void AddWarning(const std::string& stage, const std::string& reason, const ErrorLocation& location);
	void AddCriticalError(const std::string& stage, const std::string& reason);

	unsigned int GetErrorCount() const;
	unsigned int GetWarningCount() const;

	inline std::vector<Error>::const_iterator begin() const {
		return container.begin();
	}

	inline std::vector<Error>::const_iterator end() const {
		return container.end();
	}

	void Repeat(Report& report) const;
};

}
}
