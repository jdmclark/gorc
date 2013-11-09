#include "animation.h"

gorc::game::world::level::animations::animation::~animation() {
	stop();
	return;
}

void gorc::game::world::level::animations::animation::stop() {
	return;
}
