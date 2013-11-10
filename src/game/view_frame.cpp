#include "view_frame.h"

gorc::game::view_frame::view_frame(sf::Window& Window)
	: Window(Window), currentView(nullptr) {
	return;
}
