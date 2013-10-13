#include "print_event.h"

static const std::string GORC_EVENTS_PRINTEVENT_TYPE = "Gorc::Events::PrintEvent";
const std::string& Gorc::Events::PrintEvent::GetEventType() {
	return GORC_EVENTS_PRINTEVENT_TYPE;
}

Gorc::Events::PrintEvent::PrintEvent(const std::string& message) : message(message) {
	return;
}
