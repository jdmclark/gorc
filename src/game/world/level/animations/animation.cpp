#include "animation.h"

gorc::game::world::level::animations::animation::~animation() {
	stop();
	return;
}

int gorc::game::world::level::animations::animation::get_id() const {
	return -1;
}

void gorc::game::world::level::animations::animation::stop() {
	return;
}
