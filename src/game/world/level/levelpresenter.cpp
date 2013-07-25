#include "levelpresenter.h"
#include "levelmodel.h"

using namespace Gorc::Math;

Gorc::Game::World::Level::LevelPresenter::LevelPresenter(Components& components, const LevelPlace& place)
	: components(components), place(place), ScriptPresenter(components), SoundPresenter(*place.ContentManager) {
	return;
}

void Gorc::Game::World::Level::LevelPresenter::Start(Event::EventBus& eventBus) {
	model = std::unique_ptr<LevelModel>(new LevelModel(*place.ContentManager, components.Compiler, place.Level));

	AnimationPresenter.Start(*model, model->AnimationModel);
	ScriptPresenter.Start(*model, model->ScriptModel);
	SoundPresenter.Start(*model, model->SoundModel);

	InitializeWorld();

	components.LevelView.SetPresenter(this);
	components.LevelView.SetLevelModel(model.get());
	components.WorldViewFrame.SetView(components.LevelView);
	components.CurrentLevelPresenter = this;

	// Send startup and loading messages
	ScriptPresenter.SendMessageToAll(Cog::MessageId::Startup, -1, -1, Flags::MessageType::Nothing);
	ScriptPresenter.SendMessageToAll(Cog::MessageId::Loading, -1, -1, Flags::MessageType::Nothing);

	return;
}

void Gorc::Game::World::Level::LevelPresenter::InitializeWorld() {
	// HACK: Create thing collision shapes and rigid bodies, enumerate spawn points
	for(const auto& thing : model->Level.Things) {
		if(thing.Type == Flags::ThingType::Player) {
			// Add player spawn point and create ghost thing to fill ID.
			model->SpawnPoints.push_back(&thing);
			model->CreateThing("none", thing.Sector, thing.Position, thing.Orientation);
		}
		else {
			model->CreateThing(thing, thing.Sector, thing.Position, thing.Orientation);
		}
	}

	// HACK: Spawn camera thing
	model->CurrentSpawnPoint = 0;
	model->CameraThingId = model->CreateThing(*model->SpawnPoints[model->CurrentSpawnPoint], model->SpawnPoints[model->CurrentSpawnPoint]->Sector,
			model->SpawnPoints[model->CurrentSpawnPoint]->Position, model->SpawnPoints[model->CurrentSpawnPoint]->Orientation);
	auto& camera_thing = model->Things[model->CameraThingId];
	camera_thing.Flags += Flags::ThingFlag::Invisible;
	camera_thing.RigidBody->setSleepingThresholds(0.0f, 0.0f);
	camera_thing.RigidBody->setActivationState(DISABLE_DEACTIVATION);

	model->CameraPosition = camera_thing.Position;
	model->CameraSector = model->SpawnPoints[model->CurrentSpawnPoint]->Sector;

	// Create COG script instances.
	for(const auto& cog : model->Level.Cogs) {
		Content::Assets::Script const* script = std::get<0>(cog);
		const std::vector<Cog::VM::Value>& values = std::get<1>(cog);

		ScriptPresenter.CreateLevelCogInstance(script->Script, *place.ContentManager, components.Compiler, values);
	}
}

void Gorc::Game::World::Level::LevelPresenter::Update(double dt) {
	AnimationPresenter.Update(dt);
	ScriptPresenter.Update(dt);
	SoundPresenter.Update(dt);

	// Update world simulation
	model->DynamicsWorld.stepSimulation(dt, 1, 1.0f / 120.0f);

	// Update things
	btTransform trans;
	for(auto it = model->Things.begin(); it != model->Things.end(); ++it) {
		Thing& thing = *it;

		if(thing.RigidBody) {
			auto oldThingPosition = thing.Position;
			thing.RigidBody->getMotionState()->getWorldTransform(trans);
			thing.Position = VecBt(trans.getOrigin());
			UpdateThingSector(it.GetIndex(), thing, oldThingPosition);

			if(thing.PathMoving) {
				UpdateThingPathMoving(it.GetIndex(), thing, dt);
			}

			const auto& thing_sector = model->Sectors[thing.Sector];
			thing.RigidBody->applyCentralForce(BtVec(thing_sector.Thrust * RateFactor / dt));
		}
	}

	// Update camera position
	Thing& camera = model->Things[model->CameraThingId];
	model->CameraPosition = camera.Position;
	model->CameraSector = camera.Sector;

	btRigidBody& cam_body = *camera.RigidBody;
	cam_body.setAngularFactor(btVector3(0,0,0));

	auto oldLinearVelocity = VecBt(cam_body.getLinearVelocity());
	Get<2>(model->CameraVelocity) += Get<2>(oldLinearVelocity);

	auto planeCameraLinearVelocity = Vec(Get<X>(model->CameraVelocity), Get<Y>(model->CameraVelocity));
	float t = std::min(1.0f, Length(planeCameraLinearVelocity));

	auto new_camera_vel = t * model->CameraVelocity + (1.0f - t) * oldLinearVelocity;
	Get<Z>(new_camera_vel) = Get<Z>(model->CameraVelocity);
	cam_body.setLinearVelocity(BtVec(new_camera_vel));

	model->CameraVelocity = Zero<3>();
}

void Gorc::Game::World::Level::LevelPresenter::UpdateThingPathMoving(unsigned int thing_id, Thing& thing, double dt) {
	auto target_position_tuple = thing.Frames[thing.NextFrame];
	Vector<3> targetPosition = std::get<0>(target_position_tuple);
	Vector<3> orient = std::get<1>(target_position_tuple);

	static const float deg2rad = 0.0174532925f;
	btQuaternion targetOrientQuat(0.0f, 0.0f, 0.0f, 1.0f);
	targetOrientQuat *= btQuaternion(btVector3(0,0,1), deg2rad * Math::Get<1>(orient)); // Yaw
	targetOrientQuat *= btQuaternion(btVector3(1,0,0), deg2rad * Math::Get<0>(orient)); // Pitch
	targetOrientQuat *= btQuaternion(btVector3(0,1,0), deg2rad * Math::Get<2>(orient)); // Roll

	btTransform currentWorldTransform;
	thing.RigidBody->getMotionState()->getWorldTransform(currentWorldTransform);

	Vector<3> currentPosition = VecBt(currentWorldTransform.getOrigin());

	// PathMoveSpeed seems to be some factor of distance per frame, and Jedi has a different framerate.
	// Use a magic multiple to correct it.
	const float magic_number = 1.0f / 8.0f;
	float dist_len = Math::Length(targetPosition - currentPosition);
	float alpha = magic_number * dt * thing.PathMoveSpeed / dist_len;
	if(alpha >= 1.0f || dist_len == 0.0f) {
		btTransform targetWorldTransform(targetOrientQuat, BtVec(targetPosition));
		thing.RigidBody->getMotionState()->setWorldTransform(targetWorldTransform);

		// Arrived at next frame. Advance to next.
		thing.CurrentFrame = thing.NextFrame;
		if(thing.CurrentFrame == thing.GoalFrame) {
			thing.PathMoving = false;
			thing.PathMoveSpeed = 0.0f;
			ScriptPresenter.SendMessageToLinked(Cog::MessageId::Arrived, thing_id, Flags::MessageType::Thing);
			thing.RigidBody->setActivationState(0);
			SoundPresenter.StopFoleyLoop(thing_id);
			SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::StopMove);
		}
		else if(thing.CurrentFrame < thing.GoalFrame) {
			thing.NextFrame = thing.CurrentFrame + 1;
		}
		else {
			thing.NextFrame = thing.CurrentFrame - 1;
		}
	}
	else {
		auto currentQuat = currentWorldTransform.getRotation();
		if(currentQuat.angle(targetOrientQuat) < std::numeric_limits<float>::epsilon()) {
			btTransform targetWorldTransform(targetOrientQuat, currentWorldTransform.getOrigin().lerp(BtVec(targetPosition), alpha));
			thing.RigidBody->getMotionState()->setWorldTransform(targetWorldTransform);
		}
		else {
			btTransform targetWorldTransform(currentWorldTransform.getRotation(), //.slerp(targetOrientQuat, alpha),
					currentWorldTransform.getOrigin().lerp(BtVec(targetPosition), alpha));
			thing.RigidBody->getMotionState()->setWorldTransform(targetWorldTransform);
		}
	}
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

bool Gorc::Game::World::Level::LevelPresenter::InnerUpdateThingSector(Thing& thing, const Vector<3>& oldThingPosition,
		const Content::Assets::LevelSector& sector, std::vector<std::tuple<unsigned int, unsigned int>>& path) {
	if(ThingInsideSector(thing, sector)) {
		path.emplace_back(sector.Number, -1);
		return true;
	}

	for(unsigned int sec_surf_id = 0; sec_surf_id < sector.SurfaceCount; ++sec_surf_id) {
		const Content::Assets::LevelSurface& surf = model->Surfaces[sec_surf_id + sector.FirstSurface];
		if(surf.Adjoin >= 0 && !(surf.Flags & Flags::SurfaceFlag::Impassable)
				&& ThingPathPassesThroughAdjoin(thing, oldThingPosition, sector, surf)) {
			path.emplace_back(sector.Number, sec_surf_id + sector.FirstSurface);

			if(InnerUpdateThingSector(thing, oldThingPosition, model->Sectors[surf.AdjoinedSector], path)) {
				return true;
			}

			path.pop_back();
		}
	}

	return false;
}

void Gorc::Game::World::Level::LevelPresenter::UpdateThingSector(int thing_id, Thing& thing,
		const Vector<3>& oldThingPosition) {
	if(ThingInsideSector(thing, model->Sectors[thing.Sector])) {
		// Thing hasn't moved to a different sector.
		return;
	}

	std::vector<std::tuple<unsigned int, unsigned int>> path;
	if(InnerUpdateThingSector(thing, oldThingPosition, model->Sectors[thing.Sector], path)) {
		// Fire messages along path
		unsigned int first_adjoin = std::get<1>(path.front());
		if(model->Surfaces[first_adjoin].Flags & Flags::SurfaceFlag::CogLinked) {
			ScriptPresenter.SendMessageToLinked(Cog::MessageId::Crossed, first_adjoin, Flags::MessageType::Surface,
					thing_id, Flags::MessageType::Thing);
		}

		for(unsigned int i = 1; i < path.size() - 1; ++i) {
			unsigned int sec_id = std::get<0>(path[i]);
			thing.Sector = sec_id;
			thing.ObjectData.SectorId = sec_id;
			if(model->Sectors[sec_id].Flags & Flags::SectorFlag::CogLinked) {
				ScriptPresenter.SendMessageToLinked(Cog::MessageId::Entered, sec_id, Flags::MessageType::Sector,
						thing_id, Flags::MessageType::Thing);
			}

			unsigned int surf_id = std::get<1>(path[i]);
			if(model->Surfaces[surf_id].Flags & Flags::SurfaceFlag::CogLinked) {
				ScriptPresenter.SendMessageToLinked(Cog::MessageId::Crossed, surf_id, Flags::MessageType::Surface,
						thing_id, Flags::MessageType::Thing);
			}
		}

		unsigned int last_sector = std::get<0>(path.back());
		thing.Sector = last_sector;
		thing.ObjectData.SectorId = last_sector;
		if(model->Sectors[last_sector].Flags & Flags::SectorFlag::CogLinked) {
			ScriptPresenter.SendMessageToLinked(Cog::MessageId::Entered, last_sector, Flags::MessageType::Sector,
					thing_id, Flags::MessageType::Thing);
		}
	}
	else {
		// Thing hasn't moved.
		// Need to run a backup random walk here.
	}
}

void Gorc::Game::World::Level::LevelPresenter::TranslateCamera(const Vector<3>& amt) {
	Vector<3> cam_vel = Zero<3>();
	cam_vel = Zero<3>();
	cam_vel += Get<X>(amt) * Cross(model->CameraLook, model->CameraUp);
	cam_vel += Get<Z>(amt) * model->CameraUp;
	cam_vel += Get<Y>(amt) * model->CameraLook;
	cam_vel *= 0.75f;

	Get<0>(model->CameraVelocity) = Get<0>(cam_vel);
	Get<1>(model->CameraVelocity) = Get<1>(cam_vel);
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
	cameraThing.ObjectData.SectorId = model->SpawnPoints[model->CurrentSpawnPoint]->Sector;
	cameraThing.Position = model->SpawnPoints[model->CurrentSpawnPoint]->Position;
	cameraThing.RigidBody->proceedToTransform(btTransform(btQuaternion(0,0,0,1), Math::BtVec(cameraThing.Position)));
}

void Gorc::Game::World::Level::LevelPresenter::Jump() {
	Get<2>(model->CameraVelocity) = 1.6f;
}

void Gorc::Game::World::Level::LevelPresenter::Activate() {
	// TODO: Implement actual surface and thing activation

	Math::Vector<3> camera_position = model->Things[model->CameraThingId].Position;

	int best_surf_candidate = -1;
	float best_surf_dist = 0.25f;

	int best_thing_candidate = -1;
	float best_thing_dist = 0.25f;

	for(int i = 0; i < model->Surfaces.size(); ++i) {
		const Content::Assets::LevelSurface& surf = model->Surfaces[i];
		if((surf.Adjoin >= 0 && (model->Adjoins[surf.Adjoin].Flags & Flags::AdjoinFlag::AllowMovement))
				|| !(surf.Flags & Flags::SurfaceFlag::CogLinked)
				|| Math::Dot(surf.Normal, model->CameraLook) >= 0.0f) {
			continue;
		}

		for(const auto& vx : surf.Vertices) {
			float new_dist = Math::Length(camera_position - model->Level.Vertices[std::get<0>(vx)]);
			if(new_dist < best_surf_dist) {
				best_surf_candidate = i;
				best_surf_dist = new_dist;
				break;
			}
		}
	}

	for(auto it = model->Things.begin(); it != model->Things.end(); ++it) {
		auto dir_vec = it->Position - camera_position;
		if(!(it->Flags & Flags::ThingFlag::CogLinked)
				|| Math::Dot(dir_vec, model->CameraLook) <= 0.0f) {
			continue;
		}

		float dir_len = Math::Length(dir_vec);
		if(dir_len >= best_thing_dist) {
			continue;
		}

		best_thing_candidate = it.GetIndex();
		best_thing_dist = dir_len;
	}

	if(best_surf_candidate >= 0 && best_surf_dist <= best_thing_dist) {
		ScriptPresenter.SendMessageToLinked(Cog::MessageId::Activated, best_surf_candidate, Flags::MessageType::Surface,
				model->CameraThingId, Flags::MessageType::Thing);
	}
	else if(best_thing_candidate >= 0) {
		ScriptPresenter.SendMessageToLinked(Cog::MessageId::Activated, best_thing_candidate, Flags::MessageType::Thing,
				model->CameraThingId, Flags::MessageType::Thing);
		SoundPresenter.PlaySoundClass(best_thing_candidate, Flags::SoundSubclassType::Activate);
	}
}

void Gorc::Game::World::Level::LevelPresenter::Damage() {
	// TODO: Temporary copy of hack activate code from above.
	// Sends damaged message to surfaces and things for debugging.

	Math::Vector<3> camera_position = model->Things[model->CameraThingId].Position;

	int best_surf_candidate = -1;
	float best_surf_dist = 0.25f;

	int best_thing_candidate = -1;
	float best_thing_dist = 0.25f;

	for(int i = 0; i < model->Surfaces.size(); ++i) {
		const Content::Assets::LevelSurface& surf = model->Surfaces[i];
		if((surf.Adjoin >= 0 && (model->Adjoins[surf.Adjoin].Flags & Flags::AdjoinFlag::AllowMovement))
				|| !(surf.Flags & Flags::SurfaceFlag::CogLinked)
				|| Math::Dot(surf.Normal, model->CameraLook) >= 0.0f) {
			continue;
		}

		for(const auto& vx : surf.Vertices) {
			float new_dist = Math::Length(camera_position - model->Level.Vertices[std::get<0>(vx)]);
			if(new_dist < best_surf_dist) {
				best_surf_candidate = i;
				best_surf_dist = new_dist;
				break;
			}
		}
	}

	for(auto it = model->Things.begin(); it != model->Things.end(); ++it) {
		auto dir_vec = it->Position - camera_position;
		if(!(it->Flags & Flags::ThingFlag::CogLinked)
				|| Math::Dot(dir_vec, model->CameraLook) <= 0.0f) {
			continue;
		}

		float dir_len = Math::Length(dir_vec);
		if(dir_len >= best_thing_dist) {
			continue;
		}

		best_thing_candidate = it.GetIndex();
		best_thing_dist = dir_len;
	}

	if(best_surf_candidate >= 0 && best_surf_dist <= best_thing_dist) {
		ScriptPresenter.SendMessageToLinked(Cog::MessageId::Damaged, best_surf_candidate, Flags::MessageType::Surface,
				model->CameraThingId, Flags::MessageType::Thing, 1000, static_cast<int>(Flags::DamageFlag::Saber));
	}
	else if(best_thing_candidate >= 0) {
		DamageThing(best_thing_candidate, 50.0f, { Flags::DamageFlag::Saber }, model->CameraThingId);
	}
}

// Frame verbs
int Gorc::Game::World::Level::LevelPresenter::GetCurFrame(int thing_id) {
	return model->Things[thing_id].CurrentFrame;
}

void Gorc::Game::World::Level::LevelPresenter::MoveToFrame(int thing_id, int frame, float speed) {
	Thing& referenced_thing = model->Things[thing_id];

	referenced_thing.GoalFrame = frame;
	if(frame > referenced_thing.CurrentFrame) {
		referenced_thing.NextFrame = referenced_thing.CurrentFrame + 1;
	}
	else if(frame < referenced_thing.CurrentFrame) {
		referenced_thing.NextFrame = referenced_thing.CurrentFrame - 1;
	}
	else {
		referenced_thing.NextFrame = frame;
	}

	referenced_thing.PathMoveSpeed = speed;
	referenced_thing.PathMoving = true;
	SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::StartMove);
	SoundPresenter.PlayFoleyLoopClass(thing_id, Flags::SoundSubclassType::Moving);

	if(referenced_thing.RigidBody) {
		referenced_thing.RigidBody->setActivationState(DISABLE_DEACTIVATION);
	}
}

// Player verbs
int Gorc::Game::World::Level::LevelPresenter::GetLocalPlayerThing() {
	return model->CameraThingId;
}

// Sector verbs
void Gorc::Game::World::Level::LevelPresenter::SetSectorAdjoins(int sector_id, bool state) {
	Content::Assets::LevelSector& sector = model->Sectors[sector_id];
	for(unsigned int i = 0; i < sector.SurfaceCount; ++i) {
		Content::Assets::LevelSurface& surface = model->Surfaces[i + sector.FirstSurface];
		if(surface.Adjoin >= 0) {
			Content::Assets::LevelAdjoin& adjoin = model->Adjoins[surface.Adjoin];

			if(state) {
				adjoin.Flags += Flags::AdjoinFlag::Visible;
			}
			else {
				adjoin.Flags -= Flags::AdjoinFlag::Visible;
			}
		}
	}
}

void Gorc::Game::World::Level::LevelPresenter::SetSectorLight(int sector_id, float value, float delay) {
	// TODO: Create animation to implement delay feature.
	Content::Assets::LevelSector& sector = model->Sectors[sector_id];
	sector.ExtraLight = value;
}

void Gorc::Game::World::Level::LevelPresenter::SetSectorThrust(int sector_id, const Math::Vector<3>& thrust) {
	Content::Assets::LevelSector& sector = model->Sectors[sector_id];
	sector.Thrust = thrust;
}

void Gorc::Game::World::Level::LevelPresenter::SetSectorTint(int sector_id, const Math::Vector<3>& color) {
	Content::Assets::LevelSector& sector = model->Sectors[sector_id];
	sector.Tint = color;
}

// Surface verbs
void Gorc::Game::World::Level::LevelPresenter::ClearAdjoinFlags(int surface, FlagSet<Flags::AdjoinFlag> flags) {
	Content::Assets::LevelSurface& surf = model->Surfaces[surface];
	if(surf.Adjoin >= 0) {
		Content::Assets::LevelAdjoin& adj = model->Adjoins[surf.Adjoin];
		adj.Flags -= flags;
		model->UpdateSurfacePhysicsProperties(surface);
	}
}

Gorc::Math::Vector<3> Gorc::Game::World::Level::LevelPresenter::GetSurfaceCenter(int surface) {
	auto vec = Math::Zero<3>();
	for(const auto& vx : model->Level.Surfaces[surface].Vertices) {
		vec += model->Level.Vertices[std::get<0>(vx)];
	}

	vec /= static_cast<float>(model->Level.Surfaces[surface].Vertices.size());
	return vec;
}

void Gorc::Game::World::Level::LevelPresenter::SetAdjoinFlags(int surface, FlagSet<Flags::AdjoinFlag> flags) {
	Content::Assets::LevelSurface& surf = model->Surfaces[surface];
	if(surf.Adjoin >= 0) {
		Content::Assets::LevelAdjoin& adj = model->Adjoins[surf.Adjoin];
		adj.Flags += flags;
		model->UpdateSurfacePhysicsProperties(surface);
	}
}

// Thing action verbs
int Gorc::Game::World::Level::LevelPresenter::CreateThingAtThing(int tpl_id, int thing_id) {
	Thing& referencedThing = model->Things[thing_id];
	return static_cast<int>(model->CreateThing(tpl_id, referencedThing.Sector, referencedThing.Position, referencedThing.Orientation));
}

float Gorc::Game::World::Level::LevelPresenter::DamageThing(int thing_id, float damage, FlagSet<Flags::DamageFlag> flags, int damager_id) {
	ScriptPresenter.SendMessageToLinked(Cog::MessageId::Damaged, thing_id, Flags::MessageType::Thing,
			damager_id, Flags::MessageType::Thing, damage, static_cast<int>(flags));

	Thing& referencedThing = model->Things[thing_id];
	if(referencedThing.Health > 0.0f) {
		referencedThing.Health -= damage;

		if(referencedThing.Health <= 0.0f) {
			SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::Death1);
			ScriptPresenter.SendMessageToLinked(Cog::MessageId::Killed, thing_id, Flags::MessageType::Thing,
					damager_id, Flags::MessageType::Thing);
			// TODO: Thing is dead. Replace destroy code with corpse creation code.
			if(referencedThing.RigidBody) {
				model->DynamicsWorld.removeRigidBody(referencedThing.RigidBody.get());
			}

			referencedThing.Flags += Flags::ThingFlag::Invisible;
		}
		else {
			SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::HurtSpecial);
		}
	}

	// TODO: Return actual 'undamaged' value as computed by thing cog with ReturnEx.

	return 0.0f;
}

Gorc::Math::Vector<3> Gorc::Game::World::Level::LevelPresenter::GetThingPos(int thing_id) {
	Thing& referenced_thing = model->Things[thing_id];
	return referenced_thing.Position;
}

bool Gorc::Game::World::Level::LevelPresenter::IsThingMoving(int thing_id) {
	Thing& referencedThing = model->Things[thing_id];
	switch(referencedThing.Move) {
	case Flags::MoveType::Physics:
		if(referencedThing.RigidBody) {
			return referencedThing.RigidBody->getLinearVelocity().length2() > 0.0f;
		}
		else {
			return false;
		}

	case Flags::MoveType::Path:
	default:
		return false;
	}
}

// Thing property verbs
int Gorc::Game::World::Level::LevelPresenter::GetThingSector(int thing_id) {
	return model->Things[thing_id].Sector;
}

void Gorc::Game::World::Level::LevelPresenter::RegisterVerbs(Cog::Verbs::VerbTable& verbTable, Components& components) {
	Animations::AnimationPresenter::RegisterVerbs(verbTable, components);
	Scripts::ScriptPresenter::RegisterVerbs(verbTable, components);
	Sounds::SoundPresenter::RegisterVerbs(verbTable, components);

	// Frame verbs
	verbTable.AddVerb<int, 1>("getcurframe", [&components](int thing) { return components.CurrentLevelPresenter->GetCurFrame(thing); });
	verbTable.AddVerb<void, 3>("movetoframe", [&components](int thing, int frame, float speed) {
		return components.CurrentLevelPresenter->MoveToFrame(thing, frame, speed); });

	// Options verbs
	verbTable.AddVerb<int, 0>("getdifficulty", [&components] {
		// TODO: Add actual difficulty setting.
		return static_cast<int>(Flags::DifficultyMode::Medium);
	});

	// Player verbs
	verbTable.AddVerb<int, 0>("getlocalplayerthing", [&components] { return components.CurrentLevelPresenter->GetLocalPlayerThing(); });

	// Print verbs
	verbTable.AddVerb<void, 2>("jkprintunistring", [&components](int destination, const char* message) {
		// TODO: Add actual jkPrintUniString once localization is implemented.
		std::cout << message << std::endl;
	});

	verbTable.AddVerb<void, 1>("print", [&components](const char* message) {
		// TODO: Add actual print.
		std::cout << message << std::endl;
	});

	verbTable.AddVerb<void, 1>("printint", [&components](int value) {
		// TOOD: Add actual printint.
		std::cout << value << std::endl;
	});

	// Sector verbs
	verbTable.AddVerb<void, 2>("sectoradjoins", [&components](int sector_id, bool state) {
		components.CurrentLevelPresenter->SetSectorAdjoins(sector_id, state);
	});

	verbTable.AddVerb<void, 3>("sectorlight", [&components](int sector_id, float light, float delay) {
		components.CurrentLevelPresenter->SetSectorLight(sector_id, light, delay);
	});

	verbTable.AddVerb<void, 3>("sectorthrust", [&components](int sector_id, Math::Vector<3> thrust_vec, float thrust_speed) {
		components.CurrentLevelPresenter->SetSectorThrust(sector_id, Math::Normalize(thrust_vec) * thrust_speed);
	});

	verbTable.AddVerb<void, 2>("setcolormap", [&components](int sector_id, int colormap) {
		// Deliberately do nothing. (Colormaps not used after a level is loaded.)
	});

	verbTable.AddVerb<void, 2>("setsectoradjoins", [&components](int sector_id, bool state) {
		components.CurrentLevelPresenter->SetSectorAdjoins(sector_id, state);
	});

	verbTable.AddVerb<void, 2>("setsectorcolormap", [&components](int sector_id, int colormap) {
		// Deliberately do nothing. (Colormaps not used after a level is loaded.)
	});

	verbTable.AddVerb<void, 3>("setsectorlight", [&components](int sector_id, float light, float delay) {
		components.CurrentLevelPresenter->SetSectorLight(sector_id, light, delay);
	});

	verbTable.AddVerb<void, 3>("setsectorthrust", [&components](int sector_id, Math::Vector<3> thrust_vec, float thrust_speed) {
		components.CurrentLevelPresenter->SetSectorThrust(sector_id, Math::Normalize(thrust_vec) * thrust_speed);
	});

	verbTable.AddVerb<void, 2>("setsectortint", [&components](int sector_id, Math::Vector<3> tint) {
		components.CurrentLevelPresenter->SetSectorTint(sector_id, tint);
	});

	// Surface verbs
	verbTable.AddVerb<void, 2>("clearadjoinflags", [&components](int surface, int flags) {
		components.CurrentLevelPresenter->ClearAdjoinFlags(surface, FlagSet<Flags::AdjoinFlag>(flags));
	});

	verbTable.AddVerb<Math::Vector<3>, 1>("getsurfacecenter", [&components](int surface) {
		return components.CurrentLevelPresenter->GetSurfaceCenter(surface);
	});

	verbTable.AddVerb<void, 2>("setadjoinflags", [&components](int surface, int flags) {
		components.CurrentLevelPresenter->SetAdjoinFlags(surface, FlagSet<Flags::AdjoinFlag>(flags));
	});

	verbTable.AddVerb<Math::Vector<3>, 1>("surfacecenter", [&components](int surface) {
		return components.CurrentLevelPresenter->GetSurfaceCenter(surface);
	});

	// System verbs
	verbTable.AddVerb<float, 0>("rand", []{ return sf::Randomizer::Random(0.0f, 1.0f); });

	// Thing action verbs
	verbTable.AddVerb<int, 2>("creatething", [&components](int tpl_id, int thing_pos) {
		return components.CurrentLevelPresenter->CreateThingAtThing(tpl_id, thing_pos);
	});

	verbTable.AddVerb<float, 4>("damagething", [&components](int thing_id, float damage, int flags, int damager_id) {
		return components.CurrentLevelPresenter->DamageThing(thing_id, damage, FlagSet<Flags::DamageFlag>(flags), damager_id);
	});

	verbTable.AddVerb<Math::Vector<3>, 1>("getthingpos", [&components](int thing_id) {
		return components.CurrentLevelPresenter->GetThingPos(thing_id);
	});

	verbTable.AddVerb<bool, 1>("isthingmoving", [&components](int thing_id) { return components.CurrentLevelPresenter->IsThingMoving(thing_id); });
	verbTable.AddVerb<bool, 1>("ismoving", [&components](int thing_id) { return components.CurrentLevelPresenter->IsThingMoving(thing_id); });

	// Thing property verbs
	verbTable.AddVerb<int, 1>("getthingsector", [&components](int thing_id) { return components.CurrentLevelPresenter->GetThingSector(thing_id); });
}
