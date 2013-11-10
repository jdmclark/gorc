#include "shutdown_event.h"

static const std::string GORC_EVENTS_SHUTDOWNEVENT_TYPE = "gorc::events::shutdown_event";
const std::string& gorc::events::shutdown_event::get_event_type() {
	return GORC_EVENTS_SHUTDOWNEVENT_TYPE;
}
