#include "exitingevent.h"

static const std::string GORC_EVENTS_EXITINGEVENT_TYPE = "Gorc::Events::ExitingEvent";
const std::string& Gorc::Events::ExitingEvent::GetEventType() {
	return GORC_EVENTS_EXITINGEVENT_TYPE;
}

Gorc::Events::ExitingEvent::ExitingEvent() : Cancel(false) {
	return;
}
