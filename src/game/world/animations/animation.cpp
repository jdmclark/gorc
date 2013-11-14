#include "animation.h"

gorc::game::world::animations::animation::~animation() {
	stop();
	return;
}

void gorc::game::world::animations::animation::stop() {
	return;
}
