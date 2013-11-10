#pragma once

#include "report.h"
#include "error.h"

#include <vector>

namespace gorc {

namespace event {
class event_bus;
}

namespace diagnostics {

class print_event_report : public report {
private:
	event::event_bus& EventBus;
	unsigned int errorCount, warningCount;

public:
	print_event_report(event::event_bus& EventBus);

	void add_error(const std::string& stage, const std::string& reason, const error_location& location);
	void add_warning(const std::string& stage, const std::string& reason, const error_location& location);
	void add_critical_error(const std::string& stage, const std::string& reason);

	unsigned int get_error_count() const;
	unsigned int get_warning_count() const;
};

}
}
