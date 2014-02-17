#include "exit.h"

static const std::string GORC_EVENTS_EXIT_EVENT_TYPE = "gorc::events::exit";
const std::string& gorc::events::exit::get_event_type() {
    return GORC_EVENTS_EXIT_EVENT_TYPE;
}
