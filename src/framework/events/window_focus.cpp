#include "window_focus.h"

static const std::string GORC_EVENTS_WINDOWFOCUS_TYPE = "gorc::events::window_focus";
const std::string& gorc::events::window_focus::get_event_type() {
	return GORC_EVENTS_WINDOWFOCUS_TYPE;
}

gorc::events::window_focus::window_focus(bool HasFocus) : has_focus(HasFocus) {
	return;
}
