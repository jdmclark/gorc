#include "shutdown_event.h"

static const std::string GORC_EVENTS_SHUTDOWNEVENT_TYPE = "Gorc::Events::ShutdownEvent";
const std::string& Gorc::Events::ShutdownEvent::GetEventType() {
	return GORC_EVENTS_SHUTDOWNEVENT_TYPE;
}
