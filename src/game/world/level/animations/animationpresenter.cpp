#include "cog/verbs/table.h"
#include "game/components.h"
#include "animationpresenter.h"
#include "animationmodel.h"
#include "game/world/level/levelmodel.h"
#include "slideceilingskyanimation.h"
#include "slidesurfaceanimation.h"
#include "surfacematerialanimation.h"

void Gorc::Game::World::Level::Animations::AnimationPresenter::Start(LevelModel& levelModel, AnimationModel& model) {
	this->levelModel = &levelModel;
	this->model = &model;
}

void Gorc::Game::World::Level::Animations::AnimationPresenter::Update(double dt) {
	for(auto& entity : model->Animations) {
			entity->Update(dt);
	}

	for(auto it = model->Animations.begin(); it != model->Animations.end(); ++it) {
		if((*it)->Expired) {
			model->Animations.Destroy(it.GetIndex());
		}
	}
}

// Anim / Cel verbs
int Gorc::Game::World::Level::Animations::AnimationPresenter::SurfaceAnim(int surface, float rate, FlagSet<Flags::AnimFlag> flags) {
	auto ent_tuple = model->Animations.Create();
	*std::get<0>(ent_tuple) = std::unique_ptr<Animation>(new SurfaceMaterialAnimation(*levelModel, surface, rate, flags, std::get<1>(ent_tuple)));
	return std::get<1>(ent_tuple);
}

int Gorc::Game::World::Level::Animations::AnimationPresenter::GetSurfaceAnim(int surface) {
	return levelModel->Surfaces[surface].AnimNumber;
}

void Gorc::Game::World::Level::Animations::AnimationPresenter::StopAnim(int anim) {
	model->Animations.Destroy(anim);
}

int Gorc::Game::World::Level::Animations::AnimationPresenter::GetSurfaceCel(int surface) {
	return levelModel->Surfaces[surface].CelNumber;
}

void Gorc::Game::World::Level::Animations::AnimationPresenter::SetSurfaceCel(int surface, int cel) {
	levelModel->Surfaces[surface].CelNumber = cel;
}

int Gorc::Game::World::Level::Animations::AnimationPresenter::SlideSurface(int surface_id, const Math::Vector<3>& direction) {
	auto ent_tuple = model->Animations.Create();
	*std::get<0>(ent_tuple) = std::unique_ptr<Animation>(new SlideSurfaceAnimation(*levelModel, surface_id, direction));
	return std::get<1>(ent_tuple);
}

int Gorc::Game::World::Level::Animations::AnimationPresenter::SlideCeilingSky(float u_speed, float v_speed) {
	auto ent_tuple = model->Animations.Create();
	*std::get<0>(ent_tuple) = std::unique_ptr<Animation>(new SlideCeilingSkyAnimation(*levelModel, Vec(u_speed, v_speed)));
	return std::get<1>(ent_tuple);
}

void Gorc::Game::World::Level::Animations::AnimationPresenter::RegisterVerbs(Cog::Verbs::VerbTable& verbTable, Components& components) {
	verbTable.AddVerb<int, 1>("getsurfaceanim", [&components](int surface) {
		return components.CurrentLevelPresenter->AnimationPresenter.GetSurfaceAnim(surface);
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
		return components.CurrentLevelPresenter->AnimationPresenter.SurfaceAnim(surface, rate, FlagSet<Flags::AnimFlag>(flags));
	});

	verbTable.AddVerb<int, 2>("slideceilingsky", [&components](float u_speed, float v_speed) {
		return components.CurrentLevelPresenter->AnimationPresenter.SlideCeilingSky(u_speed, v_speed);
	});

	verbTable.AddVerb<int, 3>("slidesurface", [&components](int surface, Math::Vector<3> direction, float speed) {
		return components.CurrentLevelPresenter->AnimationPresenter.SlideSurface(surface, Math::Normalize(direction) * speed);
	});

	verbTable.AddVerb<int, 3>("slidewall", [&components](int surface, Math::Vector<3> direction, float speed) {
		return components.CurrentLevelPresenter->AnimationPresenter.SlideSurface(surface, Math::Normalize(direction) * speed);
	});

	verbTable.AddVerb<void, 1>("stopanim", [&components](int anim) {
		components.CurrentLevelPresenter->AnimationPresenter.StopAnim(anim);
	});
}
