#pragma once

#include "report.h"
#include "error.h"

#include <vector>

namespace gorc {
namespace diagnostics {

class stored_report : public report {
private:
	std::vector<error> container;
	unsigned int errorCount, warningCount;

public:
	stored_report();

	void add_error(const std::string& stage, const std::string& reason, const error_location& location);
	void add_warning(const std::string& stage, const std::string& reason, const error_location& location);
	void add_critical_error(const std::string& stage, const std::string& reason);

	unsigned int get_error_count() const;
	unsigned int get_warning_count() const;

	inline std::vector<error>::const_iterator begin() const {
		return container.begin();
	}

	inline std::vector<error>::const_iterator end() const {
		return container.end();
	}

	void repeat(report& report) const;
};

}
}
