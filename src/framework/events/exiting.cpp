#include "exiting.h"

static const std::string GORC_EVENTS_EXITING_EVENT_TYPE = "gorc::events::exiting";
const std::string& gorc::events::exiting::get_event_type() {
    return GORC_EVENTS_EXITING_EVENT_TYPE;
}
