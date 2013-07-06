#include "levelpresenter.h"
#include "levelmodel.h"

using namespace Gorc::Math;

Gorc::Game::World::Level::LevelCogState::LevelCogState(CogTimerState& TimerState,
		int SenderId, int SenderRef, Content::Assets::MessageType SenderType,
		int SourceRef, Content::Assets::MessageType SourceType,
		int Param0, int Param1, int Param2, int Param3)
	: TimerState(TimerState), SenderId(SenderId), SenderRef(SenderRef), SenderType(SenderType),
	  SourceRef(SourceRef), SourceType(SourceType),
	  Params({ Param0, Param1, Param2, Param3 }) {
	return;
}

Gorc::Game::World::Level::LevelPresenter::LevelPresenter(Components& components, const LevelPlace& place)
	: components(components), place(place) {
	return;
}

void Gorc::Game::World::Level::LevelPresenter::Start(Event::EventBus& eventBus) {
	model = std::unique_ptr<LevelModel>(new LevelModel(*place.ContentManager, components.Compiler, place.Level));

	components.LevelView.SetPresenter(this);
	components.LevelView.SetLevelModel(model.get());
	components.WorldViewFrame.SetView(components.LevelView);
	components.CurrentLevelPresenter = this;

	UpdateCameraAmbientSound();

	// Send startup and loading messages
	SendMessageToAll(Cog::MessageId::Startup, -1, -1, Content::Assets::MessageType::Nothing);
	SendMessageToAll(Cog::MessageId::Loading, -1, -1, Content::Assets::MessageType::Nothing);

	return;
}

void Gorc::Game::World::Level::LevelPresenter::Update(double dt) {
	// Update camera position
	Thing& camera = model->Things[model->CameraThingId];

	camera.RigidBody->applyCentralForce(BtVec(model->CameraVelocity * 800.0f));
	camera.RigidBody->setDamping(0.999f, 1.0f);

	// Update things
	btTransform trans;
	for(auto& thing : model->Things) {
		if(thing.RigidBody) {
			auto oldThingPosition = thing.Position;
			thing.RigidBody->getMotionState()->getWorldTransform(trans);
			thing.Position = VecBt(trans.getOrigin());
			UpdateThingSector(thing, oldThingPosition);
		}
	}

	// Update listener
	sf::Listener::SetTarget(0.0f, 0.0f, 1.0f);
	sf::Listener::SetPosition(Math::Get<0>(camera.Position), Math::Get<1>(camera.Position), Math::Get<2>(camera.Position));
	// TODO: Handle camera orientation (not currently properly implemented in SFML).
	UpdateCameraAmbientSound();

	// Update sounds
	for(auto& sound : model->Sounds) {
		sound.Update(dt, *model);
	}

	for(auto it = model->Sounds.begin(); it != model->Sounds.end(); ++it) {
		if(it->GetExpired()) {
			model->Sounds.Destroy(it.GetIndex());
		}
	}

	// Update animations
	for(auto& entity : model->Animations) {
		entity->Update(dt);
	}

	for(auto it = model->Animations.begin(); it != model->Animations.end(); ++it) {
		if((*it)->Expired) {
			model->Animations.Destroy(it.GetIndex());
		}
	}

	// Update cogs
	for(std::tuple<std::unique_ptr<Cog::Instance>, CogTimerState>& cog : model->Cogs) {
		Cog::Instance& inst = *std::get<0>(cog);
		CogTimerState& timer_state = std::get<1>(cog);

		if(timer_state.TimerRemainingTime > 0.0) {
			timer_state.TimerRemainingTime -= dt;
			if(timer_state.TimerRemainingTime <= 0.0) {
				timer_state.TimerRemainingTime = 0.0;
				SendMessage(inst, timer_state, Cog::MessageId::Timer, -1, -1, Content::Assets::MessageType::Nothing);
			}
		}
	}
}

void Gorc::Game::World::Level::LevelPresenter::UpdateSimulation(double dt) {
	model->DynamicsWorld.stepSimulation(dt, 20);
}

void Gorc::Game::World::Level::LevelPresenter::UpdateCameraAmbientSound() {
	const auto& sec = model->Level.Sectors[model->Things[model->CameraThingId].Sector];
	if(&sec.AmbientSound->Buffer == AmbientSound.GetBuffer() && AmbientSound.GetStatus() != sf::Sound::Stopped) {
		AmbientSound.SetVolume(sec.AmbientSoundVolume * 100.0f);
	}
	else if (sec.AmbientSound != nullptr) {
		AmbientSound.SetBuffer(sec.AmbientSound->Buffer);
		AmbientSound.SetLoop(true);
		AmbientSound.SetVolume(sec.AmbientSoundVolume * 100.0f);
		AmbientSound.SetPosition(0,0,0);
		AmbientSound.SetRelativeToListener(true);
		AmbientSound.SetAttenuation(0.0f);
		AmbientSound.Play();
	}
	else {
		AmbientSound.Stop();
	}
}

void Gorc::Game::World::Level::LevelPresenter::SetThingCurrentSector(Thing& thing, unsigned int sec_num) {
	thing.Sector = sec_num;
}

bool Gorc::Game::World::Level::LevelPresenter::ThingInsideSector(Thing& thing, const Gorc::Content::Assets::LevelSector& sec) {
	for(size_t i =  sec.FirstSurface; i < sec.FirstSurface + sec.SurfaceCount; ++i) {
		const auto& surf = model->Level.Surfaces[i];
		const auto& p = model->Level.Vertices[std::get<0>(surf.Vertices[0])];

		if(Dot(surf.Normal, thing.Position - p) < 0.0f) {
			return false;
		}
	}

	return true;
}

bool Gorc::Game::World::Level::LevelPresenter::ThingPathPassesThroughAdjoin(Thing& thing, const Vector<3>& oldThingPosition,
		const Gorc::Content::Assets::LevelSector& sec, const Gorc::Content::Assets::LevelSurface& surf) {
	if(Dot(thing.Position - oldThingPosition, surf.Normal) > 0.0f) {
		return false;
	}

	auto p = model->Level.Vertices[std::get<0>(surf.Vertices[0])];
	auto u = Dot(surf.Normal, p - oldThingPosition) / Dot(surf.Normal, thing.Position - oldThingPosition);
	if(u < 0.0f || u > 1.0f) {
		return false;
	}

	// TODO: Check for thing path passing through adjoin polygon

	return true;
}

void Gorc::Game::World::Level::LevelPresenter::UpdateThingSector(Thing& thing, const Vector<3>& oldThingPosition) {
	std::vector<size_t> sectors { thing.Sector };
	while(!sectors.empty()) {
		size_t sec_num = sectors.back();
		sectors.pop_back();

		const auto& sec = model->Level.Sectors[sec_num];

		if(ThingInsideSector(thing, sec)) {
			SetThingCurrentSector(thing, sec.Number);
			return;
		}

		for(size_t i = sec.FirstSurface; i < sec.FirstSurface + sec.SurfaceCount; ++i) {
			const auto& surf = model->Level.Surfaces[i];
			if(surf.Adjoin >= 0 && !(surf.Flags & Gorc::Content::Assets::SurfaceFlag::Impassable)
					&& ThingPathPassesThroughAdjoin(thing, oldThingPosition, sec, surf)) {
				sectors.push_back(surf.AdjoinedSector);
			}
		}
	}
}

void Gorc::Game::World::Level::LevelPresenter::SendMessage(Cog::Instance& script,
		CogTimerState& timer_state, Cog::MessageId message,
		int SenderId, int SenderRef, Content::Assets::MessageType SenderType,
		int SourceRef, Content::Assets::MessageType SourceType,
		int Param0, int Param1, int Param2, int Param3) {
	RunningCogState.emplace(timer_state, SenderId, SenderRef, SenderType, SourceRef, SourceType, Param0, Param1, Param2, Param3);

	script.Call(components.VerbTable, VirtualMachine, message);

	RunningCogState.pop();
}

void Gorc::Game::World::Level::LevelPresenter::SendMessageToAll(Cog::MessageId message,
		int SenderId, int SenderRef, Content::Assets::MessageType SenderType,
		int SourceRef, Content::Assets::MessageType SourceType,
		int Param0, int Param1, int Param2, int Param3) {
	for(auto& script : model->Cogs) {
		SendMessage(*std::get<0>(script), std::get<1>(script),
				message, SenderId, SenderRef, SenderType,
				SourceRef, SourceType, Param0, Param1, Param2, Param3);
	}
}

void Gorc::Game::World::Level::LevelPresenter::TranslateCamera(const Vector<3>& amt) {
	model->CameraVelocity = Zero<3>();
	model->CameraVelocity += Get<X>(amt) * Cross(model->CameraLook, model->CameraUp);
	model->CameraVelocity += Get<Z>(amt) * model->CameraUp;
	model->CameraVelocity += Get<Y>(amt) * model->CameraLook;
}

void Gorc::Game::World::Level::LevelPresenter::YawCamera(double amt) {
	float sint = std::sin(amt);
	float cost = std::cos(amt);

	Vector<3> NewLook = Vec(
			cost * Get<X>(model->CameraLook) - sint * Get<Y>(model->CameraLook),
			sint * Get<X>(model->CameraLook) + cost * Get<Y>(model->CameraLook),
			Get<Z>(model->CameraLook));
	Vector<3> NewUp = Vec(
			cost * Get<X>(model->CameraUp) - sint * Get<Y>(model->CameraUp),
			sint * Get<X>(model->CameraUp) + cost * Get<Y>(model->CameraUp),
			Get<Z>(model->CameraUp));

	model->CameraLook = Normalize(NewLook);
	model->CameraUp = Normalize(NewUp);
}

void Gorc::Game::World::Level::LevelPresenter::PitchCamera(double amt) {
	float sint = std::sin(amt);
	float cost = std::cos(amt);

	Vector<3> NewUp = (-sint * model->CameraLook) + (cost * model->CameraUp);
	Vector<3> NewLook = (cost * model->CameraLook) + (sint * model->CameraUp);
	model->CameraUp = Normalize(NewUp);
	model->CameraLook = Normalize(NewLook);
}

void Gorc::Game::World::Level::LevelPresenter::Respawn() {
	++model->CurrentSpawnPoint;
	model->CurrentSpawnPoint = model->CurrentSpawnPoint % model->SpawnPoints.size();

	Thing& cameraThing = model->Things[model->CameraThingId];
	cameraThing.Sector = model->SpawnPoints[model->CurrentSpawnPoint]->Sector;
	cameraThing.Position = model->SpawnPoints[model->CurrentSpawnPoint]->Position;
	cameraThing.RigidBody->proceedToTransform(btTransform(btQuaternion(0,0,0,1), Math::BtVec(cameraThing.Position)));
}

void Gorc::Game::World::Level::LevelPresenter::Activate() {
	// TODO: Implement surface and thing activation
}

// Anim / Cel verbs
int Gorc::Game::World::Level::LevelPresenter::SurfaceAnim(int surface, float rate, FlagSet<Content::Assets::SurfaceAnimationFlag> flags) {
	auto ent_tuple = model->Animations.Create();
	*std::get<0>(ent_tuple) = std::unique_ptr<Animation>(new SurfaceAnimation(*model, surface, rate, flags, std::get<1>(ent_tuple)));
	return std::get<1>(ent_tuple);
}

int Gorc::Game::World::Level::LevelPresenter::GetSurfaceAnim(int surface) {
	return model->Surfaces[surface].AnimNumber;
}

void Gorc::Game::World::Level::LevelPresenter::StopAnim(int anim) {
	model->Animations.Destroy(anim);
}

int Gorc::Game::World::Level::LevelPresenter::GetSurfaceCel(int surface) {
	return model->Surfaces[surface].CelNumber;
}

void Gorc::Game::World::Level::LevelPresenter::SetSurfaceCel(int surface, int cel) {
	model->Surfaces[surface].CelNumber = cel;
}

// Sound verbs
int Gorc::Game::World::Level::LevelPresenter::PlaySoundLocal(int wav, float volume, float panning, FlagSet<Content::Assets::SoundFlag> flags) {
	auto snd_tuple = model->Sounds.Create();

	Sound& snd = *std::get<0>(snd_tuple);
	snd.PlaySoundLocal(place.ContentManager->GetAsset<Content::Assets::Sound>(wav), volume, panning, flags);

	return std::get<1>(snd_tuple);
}

int Gorc::Game::World::Level::LevelPresenter::PlaySoundPos(int wav, Math::Vector<3> pos, float volume, float minrad, float maxrad,
		FlagSet<Content::Assets::SoundFlag> flags) {
	auto snd_tuple = model->Sounds.Create();

	Sound& snd = *std::get<0>(snd_tuple);
	snd.PlaySoundPos(place.ContentManager->GetAsset<Content::Assets::Sound>(wav), pos, volume, minrad, maxrad, flags);

	return std::get<1>(snd_tuple);
}

int Gorc::Game::World::Level::LevelPresenter::PlaySoundThing(int wav, int thing, float volume, float minrad, float maxrad,
		FlagSet<Content::Assets::SoundFlag> flags) {
	auto snd_tuple = model->Sounds.Create();

	Sound& snd = *std::get<0>(snd_tuple);
	snd.PlaySoundThing(*model, place.ContentManager->GetAsset<Content::Assets::Sound>(wav), thing, volume, minrad, maxrad, flags);

	return std::get<1>(snd_tuple);
}

// Surface verbs
Gorc::Math::Vector<3> Gorc::Game::World::Level::LevelPresenter::GetSurfaceCenter(int surface) {
	auto vec = Math::Zero<3>();
	for(const auto& vx : model->Level.Surfaces[surface].Vertices) {
		vec += model->Level.Vertices[std::get<0>(vx)];
	}

	vec /= static_cast<float>(model->Level.Surfaces[surface].Vertices.size());
	return vec;
}

// Thing verbs

int Gorc::Game::World::Level::LevelPresenter::CreateThingAtThing(int tpl_id, int thing_id) {
	Thing& referencedThing = model->Things[thing_id];
	return static_cast<int>(model->CreateThing(tpl_id, referencedThing.Sector, referencedThing.Position, referencedThing.Orientation));
}
