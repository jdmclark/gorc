#pragma once

#include "report.h"
#include "error.h"

#include <vector>

namespace Gorc {

namespace Event {
class EventBus;
}

namespace Diagnostics {

class PrintEventReport : public Report {
private:
	Event::EventBus& EventBus;
	unsigned int errorCount, warningCount;

public:
	PrintEventReport(Event::EventBus& EventBus);

	void AddError(const std::string& stage, const std::string& reason, const ErrorLocation& location);
	void AddWarning(const std::string& stage, const std::string& reason, const ErrorLocation& location);
	void AddCriticalError(const std::string& stage, const std::string& reason);

	unsigned int GetErrorCount() const;
	unsigned int GetWarningCount() const;
};

}
}
