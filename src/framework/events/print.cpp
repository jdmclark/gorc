#include "print.h"

static const std::string GORC_EVENTS_PRINT_EVENT_TYPE = "gorc::events::print";
const std::string& gorc::events::print::get_event_type() {
	return GORC_EVENTS_PRINT_EVENT_TYPE;
}

gorc::events::print::print(const std::string& message) : message(message) {
	return;
}
