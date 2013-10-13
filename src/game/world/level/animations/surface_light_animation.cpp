#include "surface_light_animation.h"
#include "game/world/level/level_model.h"

Gorc::Game::World::Level::Animations::SurfaceLightAnimation::SurfaceLightAnimation(LevelModel& model, unsigned int surface, float start_light,
		float end_light, float change_time, int anim_num)
	: model(model), surface(surface), start_light(start_light), end_light(end_light), change_time(change_time), anim_time(0.0) {
	model.Surfaces[surface].ExtraLight = start_light;
	model.Surfaces[surface].AnimNumber = anim_num;
	return;
}

void Gorc::Game::World::Level::Animations::SurfaceLightAnimation::Update(double dt) {
	// TODO: It may have been LEC's intention for this animation to loop.
	// Test it out, see if it was a bug all along.
	anim_time += static_cast<float>(dt);
	float alpha = std::min(anim_time / change_time, 1.0f);
	model.Surfaces[surface].ExtraLight = (1.0f - alpha) * start_light + alpha * end_light;
}

void Gorc::Game::World::Level::Animations::SurfaceLightAnimation::Stop() {
	auto& surf = model.Surfaces[surface];
	surf.AnimNumber = -1;
}
