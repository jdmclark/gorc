#include "surfacematerialanimation.h"
#include "game/world/level/levelmodel.h"

Gorc::Game::World::Level::Animations::SurfaceMaterialAnimation::SurfaceMaterialAnimation(LevelModel& model, unsigned int surface,
		double framerate, FlagSet<Flags::AnimFlag> flag, Id<PoolPtr<Animation>> anim_num)
	: model(model), surface(surface), framerate(1.0 / framerate), flag(flag), framerate_accumulator(0.0) {

	int surface_material = model.Level.Surfaces[surface].Material;
	if(surface_material < 0) {
		// TODO: Surface has no material but has an animation? Report error.
		num_cels = 0;
		return;
	}

	num_cels = std::get<0>(model.Level.Materials[surface_material])->Cels.size();

	if(flag & Flags::AnimFlag::SkipFirstTwoFrames) {
		model.Surfaces[surface].CelNumber = 2 % num_cels;
	}
	else if(flag & Flags::AnimFlag::SkipFirstFrame) {
		model.Surfaces[surface].CelNumber = 1 % num_cels;
	}
	else {
		model.Surfaces[surface].CelNumber = 0;
	}

	model.Surfaces[surface].AnimNumber = anim_num;
}

void Gorc::Game::World::Level::Animations::SurfaceMaterialAnimation::Update(double dt) {
	framerate_accumulator += dt;

	while(framerate_accumulator >= framerate) {
		framerate_accumulator -= framerate;

		int next_cel = model.Surfaces[surface].CelNumber + 1;
		if(next_cel >= num_cels) {
			if(flag & Flags::AnimFlag::SkipFirstTwoFrames) {
				next_cel = 2 % num_cels;
			}
			else if(flag & Flags::AnimFlag::SkipFirstFrame) {
				next_cel = 1 % num_cels;
			}
			else {
				next_cel = 0;
			}
		}

		model.Surfaces[surface].CelNumber = next_cel;
	}
}

void Gorc::Game::World::Level::Animations::SurfaceMaterialAnimation::Stop() {
	model.Surfaces[surface].AnimNumber = Id<PoolPtr<Animation>>();
}
