#include "slide_ceiling_sky_animation.h"
#include "game/world/level/level_model.h"
#include "game/constants.h"

gorc::game::world::level::animations::slide_ceiling_sky_animation::slide_ceiling_sky_animation(level_model& model, const vector<2>& speed)
	: model(model), speed(speed) {
	return;
}

void gorc::game::world::level::animations::slide_ceiling_sky_animation::update(double dt) {
	model.header.ceiling_sky_offset += speed * dt * rate_factor;
}
