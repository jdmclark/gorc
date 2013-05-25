#pragma once

#include "errorlocation.h"

#include <string>

namespace Gorc {
namespace Diagnostics {

class Report {
public:
	virtual ~Report();

	virtual void AddError(const std::string& stage, const std::string& reason, const ErrorLocation& location) = 0;
	virtual void AddWarning(const std::string& stage, const std::string& reason, const ErrorLocation& location) = 0;
	virtual void AddCriticalError(const std::string& stage, const std::string& reason) = 0;

	virtual unsigned int GetErrorCount() const = 0;
	virtual unsigned int GetWarningCount() const = 0;
};

}
}
