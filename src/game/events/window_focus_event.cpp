#include "window_focus_event.h"

static const std::string GORC_GAME_EVENTS_WINDOWFOCUSEVENT_TYPE = "Gorc::Game::Events::WindowFocusEvent";
const std::string& Gorc::Game::Events::WindowFocusEvent::GetEventType() {
	return GORC_GAME_EVENTS_WINDOWFOCUSEVENT_TYPE;
}

Gorc::Game::Events::WindowFocusEvent::WindowFocusEvent(bool HasFocus) : HasFocus(HasFocus) {
	return;
}
