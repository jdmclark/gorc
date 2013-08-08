#include "surfacelightanimation.h"
#include "game/world/level/levelmodel.h"

Gorc::Game::World::Level::Animations::SurfaceLightAnimation::SurfaceLightAnimation(LevelModel& model, unsigned int surface, float start_light,
		float end_light, float change_time)
	: model(model), surface(surface), start_light(start_light), end_light(end_light), change_time(change_time), anim_time(0.0) {
	model.Surfaces[surface].ExtraLight = start_light;
	return;
}

void Gorc::Game::World::Level::Animations::SurfaceLightAnimation::Update(double dt) {
	// TODO: It may have been LEC's intention for this animation to loop.
	// Test it out, see if it was a bug all along.
	anim_time += static_cast<float>(dt);
	float alpha = std::min(anim_time / change_time, 1.0f);
	model.Surfaces[surface].ExtraLight = (1.0f - alpha) * start_light + alpha * end_light;
}
