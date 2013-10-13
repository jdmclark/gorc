#include "view_frame.h"

Gorc::Game::ViewFrame::ViewFrame(sf::Window& Window)
	: Window(Window), currentView(nullptr) {
	return;
}
