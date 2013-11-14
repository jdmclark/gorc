#include "terminated.h"

static const std::string GORC_EVENTS_TERMINATED_EVENT_TYPE = "gorc::events::terminated";
const std::string& gorc::events::terminated::get_event_type() {
	return GORC_EVENTS_TERMINATED_EVENT_TYPE;
}
