#include "print_event.h"

static const std::string GORC_EVENTS_PRINTEVENT_TYPE = "gorc::events::print_event";
const std::string& gorc::events::print_event::get_event_type() {
	return GORC_EVENTS_PRINTEVENT_TYPE;
}

gorc::events::print_event::print_event(const std::string& message) : message(message) {
	return;
}
