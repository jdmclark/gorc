#include "slidesurfaceanimation.h"
#include "game/world/level/levelmodel.h"

Gorc::Game::World::Level::Animations::SlideSurfaceAnimation::SlideSurfaceAnimation(LevelModel& model, unsigned int surface, const Math::Vector<3>& direction)
	: model(model), surface(surface), direction(direction) {
	return;
}

void Gorc::Game::World::Level::Animations::SlideSurfaceAnimation::Update(double dt) {
	model.Surfaces[surface].TextureOffset += Math::Vec(Math::Get<0>(direction), Math::Get<1>(direction)) * dt;
}
