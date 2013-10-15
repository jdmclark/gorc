#include "window_focus_event.h"

static const std::string GORC_GAME_EVENTS_WINDOWFOCUSEVENT_TYPE = "gorc::game::events::window_focus_event";
const std::string& gorc::game::events::window_focus_event::get_event_type() {
	return GORC_GAME_EVENTS_WINDOWFOCUSEVENT_TYPE;
}

gorc::game::events::window_focus_event::window_focus_event(bool HasFocus) : has_focus(HasFocus) {
	return;
}
