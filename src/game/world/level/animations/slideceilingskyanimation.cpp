#include "slideceilingskyanimation.h"
#include "game/world/level/levelmodel.h"
#include "game/constants.h"

Gorc::Game::World::Level::Animations::SlideCeilingSkyAnimation::SlideCeilingSkyAnimation(LevelModel& model, const Math::Vector<2>& speed)
	: model(model), speed(speed) {
	return;
}

void Gorc::Game::World::Level::Animations::SlideCeilingSkyAnimation::Update(double dt) {
	model.Header.CeilingSkyOffset += speed * dt * RateFactor;
}
