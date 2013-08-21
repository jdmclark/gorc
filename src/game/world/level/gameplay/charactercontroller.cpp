#include "charactercontroller.h"
#include "game/world/level/levelpresenter.h"
#include "game/world/level/levelmodel.h"
#include "game/constants.h"

void Gorc::Game::World::Level::Gameplay::CharacterController::PlayRunningAnimation(int thing_id, Thing& thing, double speed) {
	if(thing.ActorWalkAnimation >= 0) {
		Keys::KeyState& keyState = presenter.Model->KeyModel.Keys[thing.ActorWalkAnimation];
		const Content::Assets::PuppetSubmode& submode = thing.Puppet->GetMode(Flags::PuppetModeType::Default).GetSubmode(Flags::PuppetSubmodeType::Run);
		if(keyState.Animation != submode.Animation) {
			keyState.AnimationTime = 0.0;
		}

		keyState.Animation = submode.Animation;
		keyState.HighPriority = submode.HiPriority;
		keyState.LowPriority = submode.LoPriority;
		keyState.Flags = FlagSet<Flags::KeyFlag>();
		keyState.Speed = speed;
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::PlayStandingAnimation(int thing_id, Thing& thing) {
	if(thing.ActorWalkAnimation >= 0) {
		Keys::KeyState& keyState = presenter.Model->KeyModel.Keys[thing.ActorWalkAnimation];
		const Content::Assets::PuppetSubmode& submode = thing.Puppet->GetMode(Flags::PuppetModeType::Default).GetSubmode(Flags::PuppetSubmodeType::Stand);
		keyState.Speed = 1.0;
		keyState.Animation = submode.Animation;
		keyState.HighPriority = submode.HiPriority;
		keyState.LowPriority = submode.LoPriority;
		keyState.Flags = FlagSet<Flags::KeyFlag>();
	}
}

Gorc::Game::World::Level::Gameplay::StandingMaterial Gorc::Game::World::Level::Gameplay::CharacterController::GetStandingMaterial(Thing& thing) {
	if(thing.AttachFlags & Flags::AttachFlag::AttachedToThingFace) {
		auto& floor_thing = presenter.Model->Things[thing.AttachedThing];
		if(floor_thing.Flags & Flags::ThingFlag::Metal) {
			return StandingMaterial::Metal;
		}
		else if(floor_thing.Flags & Flags::ThingFlag::Dirt) {
			return StandingMaterial::Dirt;
		}
		else {
			return StandingMaterial::Hard;
		}
	}
	else if(thing.AttachFlags & Flags::AttachFlag::AttachedToWorldSurface) {
		auto& floor_surf = presenter.Model->Surfaces[thing.AttachedSurface];

		if(floor_surf.Flags & Flags::SurfaceFlag::Metal) {
			return StandingMaterial::Metal;
		}
		else if(floor_surf.Flags & Flags::SurfaceFlag::Dirt) {
			return StandingMaterial::Dirt;
		}
		else if(floor_surf.Flags & Flags::SurfaceFlag::ShallowWater) {
			return StandingMaterial::ShallowWater;
		}
		else if(floor_surf.Flags & Flags::SurfaceFlag::DeepWater) {
			return StandingMaterial::DeepWater;
		}
		else if(floor_surf.Flags & Flags::SurfaceFlag::VeryDeepWater) {
			return StandingMaterial::VeryDeepWater;
		}
		else {
			return StandingMaterial::Hard;
		}
	}
	else {
		return StandingMaterial::None;
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::Update(int thing_id, double dt) {
	ThingController::Update(thing_id, dt);
}

void Gorc::Game::World::Level::Gameplay::CharacterController::CreateControllerData(int thing_id) {
	auto& new_thing = presenter.Model->Things[thing_id];

	// HACK: Initialize actor walk animation
	if(new_thing.Puppet) {
		new_thing.ActorWalkAnimation = presenter.KeyPresenter.PlayPuppetKey(thing_id, Flags::PuppetModeType::Default, Flags::PuppetSubmodeType::Stand);
		Keys::KeyState& keyState = presenter.Model->KeyModel.Keys[new_thing.ActorWalkAnimation];
		keyState.Flags = FlagSet<Flags::KeyFlag>();
	}
	else {
		new_thing.ActorWalkAnimation = -1;
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::HandleAnimationMarker(int thing_id, Flags::KeyMarkerType marker) {
	switch(marker) {
	case Flags::KeyMarkerType::LeftRunFootstep:
		PlayLeftRunFootstep(thing_id);
		break;

	case Flags::KeyMarkerType::RightRunFootstep:
		PlayRightRunFootstep(thing_id);
		break;
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::PlayLeftRunFootstep(int thing_id) {
	auto& thing = presenter.Model->Things[thing_id];
	StandingMaterial mat = GetStandingMaterial(thing);

	switch(mat) {
	case StandingMaterial::Metal:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::LRunMetal);
		break;

	case StandingMaterial::Dirt:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::LRunEarth);
		break;

	case StandingMaterial::ShallowWater:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::LRunPuddle);
		break;

	case StandingMaterial::DeepWater:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::LRunWater);
		break;

	case StandingMaterial::VeryDeepWater:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::LRunWater);
		break;

	default:
	case StandingMaterial::Hard:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::LRunHard);
		break;
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::PlayRightRunFootstep(int thing_id) {
	auto& thing = presenter.Model->Things[thing_id];
	StandingMaterial mat = GetStandingMaterial(thing);

	switch(mat) {
	case StandingMaterial::Metal:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::RRunMetal);
		break;

	case StandingMaterial::Dirt:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::RRunEarth);
		break;

	case StandingMaterial::ShallowWater:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::RRunPuddle);
		break;

	case StandingMaterial::DeepWater:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::RRunWater);
		break;

	case StandingMaterial::VeryDeepWater:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::RRunWater);
		break;

	default:
	case StandingMaterial::Hard:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::RRunHard);
		break;
	}
}
