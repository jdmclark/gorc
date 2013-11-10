#include "exiting_event.h"

static const std::string GORC_EVENTS_EXITINGEVENT_TYPE = "gorc::events::exiting_event";
const std::string& gorc::events::exiting_event::get_event_type() {
	return GORC_EVENTS_EXITINGEVENT_TYPE;
}

gorc::events::exiting_event::exiting_event() : cancel(false) {
	return;
}
