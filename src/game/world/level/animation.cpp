#include "animation.h"
#include "levelmodel.h"

Gorc::Game::World::Level::Animation::~Animation() {
	return;
}

void Gorc::Game::World::Level::Animation::Stop() {
	return;
}

Gorc::Game::World::Level::SurfaceAnimation::SurfaceAnimation(LevelModel& model, unsigned int surface, double framerate,
		FlagSet<Content::Assets::SurfaceAnimationFlag> flag, int anim_num)
	: model(model), surface(surface), framerate(1.0 / framerate), flag(flag), framerate_accumulator(0.0) {

	int surface_material = model.Level.Surfaces[surface].Material;
	if(surface_material < 0) {
		// TODO: Surface has no material but has an animation? Report error.
		num_cels = 0;
		return;
	}

	num_cels = std::get<0>(model.Level.Materials[surface_material])->Cels.size();

	if(flag & Content::Assets::SurfaceAnimationFlag::SkipFirstTwoFrames) {
		model.Surfaces[surface].CelNumber = 2 % num_cels;
	}
	else if(flag & Content::Assets::SurfaceAnimationFlag::SkipFirstFrame) {
		model.Surfaces[surface].CelNumber = 1 % num_cels;
	}
	else {
		model.Surfaces[surface].CelNumber = 0;
	}

	model.Surfaces[surface].AnimNumber = anim_num;
}

void Gorc::Game::World::Level::SurfaceAnimation::Update(double dt) {
	framerate_accumulator += dt;

	while(framerate_accumulator >= framerate) {
		framerate_accumulator -= framerate;

		int next_cel = model.Surfaces[surface].CelNumber + 1;
		if(next_cel >= num_cels) {
			if(flag & Content::Assets::SurfaceAnimationFlag::SkipFirstTwoFrames) {
				next_cel = 2 % num_cels;
			}
			else if(flag & Content::Assets::SurfaceAnimationFlag::SkipFirstFrame) {
				next_cel = 1 % num_cels;
			}
			else {
				next_cel = 0;
			}
		}

		model.Surfaces[surface].CelNumber = next_cel;
	}
}

void Gorc::Game::World::Level::SurfaceAnimation::Stop() {
	model.Surfaces[surface].AnimNumber = -1;
}

Gorc::Game::World::Level::SlideSurfaceAnimation::SlideSurfaceAnimation(LevelModel& model, unsigned int surface, const Math::Vector<3>& direction)
	: model(model), surface(surface), direction(direction) {
	return;
}

void Gorc::Game::World::Level::SlideSurfaceAnimation::Update(double dt) {
	model.Surfaces[surface].TextureOffset += Math::Vec(Math::Get<0>(direction), Math::Get<1>(direction)) * dt;
}

Gorc::Game::World::Level::CeilingSkyAnimation::CeilingSkyAnimation(LevelModel& model, const Math::Vector<2>& speed)
	: model(model), speed(speed) {
	return;
}

void Gorc::Game::World::Level::CeilingSkyAnimation::Update(double dt) {
	model.Header.CeilingSkyOffset += speed * dt;
}
