#include "exit_event.h"

static const std::string GORC_EVENTS_EXITEVENT_TYPE = "gorc::events::exit_event";
const std::string& gorc::events::exit_event::get_event_type() {
	return GORC_EVENTS_EXITEVENT_TYPE;
}
