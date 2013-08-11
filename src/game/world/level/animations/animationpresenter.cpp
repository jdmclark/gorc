#include "cog/verbs/table.h"
#include "game/components.h"
#include "animationpresenter.h"
#include "animationmodel.h"
#include "game/world/level/levelmodel.h"
#include "slideceilingskyanimation.h"
#include "slidesurfaceanimation.h"
#include "surfacematerialanimation.h"
#include "surfacelightanimation.h"

using namespace Gorc::Game::World::Level::Animations;

void AnimationPresenter::Start(LevelModel& levelModel, AnimationModel& model) {
	this->levelModel = &levelModel;
	this->model = &model;
}

void AnimationPresenter::Update(double dt) {
	for(auto& entity : model->Animations) {
			entity->Update(dt);
	}

	for(auto& anim : model->Animations) {
		if(anim->Expired) {
			model->Animations.Destroy(anim);
		}
	}
}

// Anim / Cel verbs
int AnimationPresenter::SurfaceAnim(int surface, float rate, FlagSet<Flags::AnimFlag> flags) {
	auto& ent = model->Animations.Create();
	ent.Value = std::unique_ptr<Animation>(new SurfaceMaterialAnimation(*levelModel, surface, rate, flags, ent.GetId()));
	return ent.GetId();
}

int AnimationPresenter::GetSurfaceAnim(int surface) {
	return levelModel->Surfaces[surface].AnimNumber;
}

void AnimationPresenter::StopAnim(int anim) {
	if(anim >= 0) {
		model->Animations.Destroy(anim);
	}
}

int AnimationPresenter::GetSurfaceCel(int surface) {
	return levelModel->Surfaces[surface].CelNumber;
}

void AnimationPresenter::SetSurfaceCel(int surface, int cel) {
	levelModel->Surfaces[surface].CelNumber = cel;
}

int AnimationPresenter::SlideSurface(int surface_id, const Math::Vector<3>& direction) {
	auto& ent = model->Animations.Create();
	ent.Value = std::unique_ptr<Animation>(new SlideSurfaceAnimation(*levelModel, surface_id, direction, ent.GetId()));
	return ent.GetId();
}

int AnimationPresenter::SlideCeilingSky(float u_speed, float v_speed) {
	auto& ent = model->Animations.Create();
	ent.Value = std::unique_ptr<Animation>(new SlideCeilingSkyAnimation(*levelModel, Vec(u_speed, v_speed)));
	return ent.GetId();
}

int AnimationPresenter::SurfaceLightAnim(int surface, float start_light, float end_light, float change_time) {
	auto& ent = model->Animations.Create();
	ent.Value = std::unique_ptr<Animation>(new SurfaceLightAnimation(*levelModel, surface, start_light, end_light, change_time, ent.GetId()));
	return ent.GetId();
}

void Gorc::Game::World::Level::Animations::AnimationPresenter::RegisterVerbs(Cog::Verbs::VerbTable& verbTable, Components& components) {
	verbTable.AddVerb<int, 1>("getsurfaceanim", [&components](int surface) {
		return static_cast<int>(components.CurrentLevelPresenter->AnimationPresenter.GetSurfaceAnim(surface));
	});

	verbTable.AddVerb<void, 1>("stopsurfaceanim", [&components](int surface) {
		components.CurrentLevelPresenter->AnimationPresenter.StopAnim(
				components.CurrentLevelPresenter->AnimationPresenter.GetSurfaceAnim(surface));
	});

	verbTable.AddVerb<int, 1>("getwallcel", [&components](int surface) {
		return components.CurrentLevelPresenter->AnimationPresenter.GetSurfaceCel(surface);
	});

	verbTable.AddVerb<int, 2>("setwallcel", [&components](int surface, int cel) {
		components.CurrentLevelPresenter->AnimationPresenter.SetSurfaceCel(surface, cel);
		return 1;
	});

	verbTable.AddVerb<int, 3>("surfaceanim", [&components](int surface, float rate, int flags) {
		return static_cast<int>(components.CurrentLevelPresenter->AnimationPresenter.SurfaceAnim(surface, rate, FlagSet<Flags::AnimFlag>(flags)));
	});

	verbTable.AddVerb<int, 2>("slideceilingsky", [&components](float u_speed, float v_speed) {
		return static_cast<int>(components.CurrentLevelPresenter->AnimationPresenter.SlideCeilingSky(u_speed, v_speed));
	});

	verbTable.AddVerb<int, 3>("slidesurface", [&components](int surface, Math::Vector<3> direction, float speed) {
		return static_cast<int>(components.CurrentLevelPresenter->AnimationPresenter.SlideSurface(surface, Math::Normalize(direction) * speed));
	});

	verbTable.AddVerb<int, 3>("slidewall", [&components](int surface, Math::Vector<3> direction, float speed) {
		return static_cast<int>(components.CurrentLevelPresenter->AnimationPresenter.SlideSurface(surface, Math::Normalize(direction) * speed));
	});

	verbTable.AddVerb<void, 1>("stopanim", [&components](int anim) {
		components.CurrentLevelPresenter->AnimationPresenter.StopAnim(anim);
	});

	verbTable.AddVerb<int, 4>("surfacelightanim", [&components](int surface, float start_light, float end_light, float change_time) {
		return static_cast<int>(components.CurrentLevelPresenter->AnimationPresenter.SurfaceLightAnim(surface, start_light, end_light, change_time));
	});
}
