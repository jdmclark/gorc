#include "exit_event.h"

static const std::string GORC_EVENTS_EXITEVENT_TYPE = "Gorc::Events::ExitEvent";
const std::string& Gorc::Events::ExitEvent::GetEventType() {
	return GORC_EVENTS_EXITEVENT_TYPE;
}
