#include "levelpresenter.h"
#include "levelmodel.h"
#include "game/constants.h"

using namespace Gorc::Math;

Gorc::Game::World::Level::LevelPresenter::LevelPresenter(Components& components, const LevelPlace& place)
	: components(components), place(place),
	  ScriptPresenter(components), SoundPresenter(*place.ContentManager),  KeyPresenter(*place.ContentManager),
	  InventoryPresenter(*this),
	  ActorController(*this), PlayerController(*this), CogController(*this), GhostController(*this),
	  ItemController(*this), CorpseController(*this) {

	return;
}

void Gorc::Game::World::Level::LevelPresenter::Start(Event::EventBus& eventBus) {
	components.CurrentLevelPresenter = this;

	Model = std::unique_ptr<LevelModel>(new LevelModel(*place.ContentManager, components.Compiler, place.Level,
			place.ContentManager->Load<Content::Assets::Inventory>("items.dat", components.Compiler)));
	Model->DynamicsWorld.setInternalTickCallback([](btDynamicsWorld* world, btScalar timeStep) {
		reinterpret_cast<LevelPresenter*>(world->getWorldUserInfo())->PhysicsTickUpdate(timeStep);
	}, this);

	AnimationPresenter.Start(*Model, Model->AnimationModel);
	ScriptPresenter.Start(*Model, Model->ScriptModel);
	SoundPresenter.Start(*Model, Model->SoundModel);
	KeyPresenter.Start(*Model, Model->KeyModel);
	InventoryPresenter.Start(*Model, Model->InventoryModel);

	InitializeWorld();

	components.LevelView.SetPresenter(this);
	components.LevelView.SetLevelModel(Model.get());
	components.WorldViewFrame.SetView(components.LevelView);

	// Send startup and loading messages
	ScriptPresenter.SendMessageToAll(Cog::MessageId::Loading, -1, -1, Flags::MessageType::Nothing);

	return;
}

void Gorc::Game::World::Level::LevelPresenter::InitializeWorld() {
	ScriptPresenter.CreateLevelDummyInstances(Model->Level.Cogs.size());

	// HACK: Create thing collision shapes and rigid bodies, enumerate spawn points
	for(const auto& thing : Model->Level.Things) {
		if(thing.Type == Flags::ThingType::Player) {
			// Add player spawn point and create ghost thing to fill ID.
			Model->SpawnPoints.push_back(&thing);
			CreateThing("none", thing.Sector, thing.Position, thing.Orientation);
		}
		else {
			CreateThing(thing, thing.Sector, thing.Position, thing.Orientation);
		}
	}

	// HACK: Spawn camera thing
	Model->CurrentSpawnPoint = 0;
	Model->CameraThingId = CreateThing(*Model->SpawnPoints[Model->CurrentSpawnPoint], Model->SpawnPoints[Model->CurrentSpawnPoint]->Sector,
			Model->SpawnPoints[Model->CurrentSpawnPoint]->Position, Model->SpawnPoints[Model->CurrentSpawnPoint]->Orientation);
	auto& camera_thing = Model->Things[Model->CameraThingId];
	camera_thing.Flags += Flags::ThingFlag::Invisible;
	camera_thing.RigidBody->setSleepingThresholds(0.0f, 0.0f);
	camera_thing.RigidBody->setActivationState(DISABLE_DEACTIVATION);

	Model->CameraPosition = camera_thing.Position;
	Model->CameraSector = Model->SpawnPoints[Model->CurrentSpawnPoint]->Sector;

	// Create bin script instances.
	for(const auto& bin_tuple : Model->InventoryModel.BaseInventory) {
		const auto& bin = std::get<1>(bin_tuple);

		if(bin.Cog) {
			ScriptPresenter.CreateGlobalCogInstance(bin.Cog->Script, *place.ContentManager, components.Compiler);
		}
	}

	// Create COG script instances.
	for(unsigned int i = 0; i < Model->Level.Cogs.size(); ++i) {
		const auto& cog = Model->Level.Cogs[i];
		Content::Assets::Script const* script = std::get<0>(cog);
		const std::vector<Cog::VM::Value>& values = std::get<1>(cog);

		if(script) {
			ScriptPresenter.CreateLevelCogInstance(i, script->Script, *place.ContentManager, components.Compiler, values);
		}
	}
}

void Gorc::Game::World::Level::LevelPresenter::Update(double dt) {
	Model->DynamicsWorld.stepSimulation(dt, 1, GameplayTick);
}

void Gorc::Game::World::Level::LevelPresenter::PhysicsTickUpdate(double dt) {
	AnimationPresenter.Update(dt);
	ScriptPresenter.Update(dt);
	SoundPresenter.Update(dt);
	KeyPresenter.Update(dt);
	InventoryPresenter.Update(dt);

	// Update things
	btTransform trans;
	for(auto& thing : Model->Things) {
		if(thing.RigidBody) {
			auto oldThingPosition = thing.Position;
			thing.RigidBody->getMotionState()->getWorldTransform(trans);
			thing.Position = VecBt(trans.getOrigin() * PhysicsInvWorldScale);
			UpdateThingSector(thing.GetId(), thing, oldThingPosition);
		}

		thing.Controller->Update(thing.GetId(), dt);
	}

	// Perform object-object collision test and message dispatch
	int num_manifolds = Model->DynamicsWorld.getDispatcher()->getNumManifolds();
	for(int i = 0; i < num_manifolds; ++i) {
		btPersistentManifold* contactManifold = Model->DynamicsWorld.getDispatcher()->getManifoldByIndexInternal(i);

		if(contactManifold->getNumContacts() > 0) {
			PhysicsObjectData* obj_a = reinterpret_cast<PhysicsObjectData*>(
					static_cast<btCollisionObject*>(contactManifold->getBody0())->getUserPointer());
			PhysicsObjectData* obj_b = reinterpret_cast<PhysicsObjectData*>(
					static_cast<btCollisionObject*>(contactManifold->getBody1())->getUserPointer());

			ThingObjectData* thing_a;
			ThingObjectData* thing_b;
			SurfaceObjectData* surf;

			if(MatchObjectPair(obj_a, obj_b, &thing_a, &surf)) {
				// Thing colliding with surface.
				ScriptPresenter.SendMessageToLinked(Cog::MessageId::Touched,
						surf->SurfaceId, Flags::MessageType::Surface,
						thing_a->ThingId, Flags::MessageType::Thing);
			}
			else if(MatchObjectPair(obj_a, obj_b, &thing_a, &thing_b)) {
				// Thing colliding with thing.
				ScriptPresenter.SendMessageToLinked(Cog::MessageId::Touched,
						thing_a->ThingId, Flags::MessageType::Thing,
						thing_b->ThingId, Flags::MessageType::Thing);
				ScriptPresenter.SendMessageToLinked(Cog::MessageId::Touched,
						thing_b->ThingId, Flags::MessageType::Thing,
						thing_a->ThingId, Flags::MessageType::Thing);
			}
		}
	}

	// Update camera position
	UpdateCamera();

	// Update dynamic tint, game time.
	Model->GameTime += dt;
	Model->LevelTime += dt;

	Model->DynamicTint = Model->DynamicTint * (1.0 - dt);
	Math::Get<0>(Model->DynamicTint) = std::max(Math::Get<0>(Model->DynamicTint), 0.0f);
	Math::Get<1>(Model->DynamicTint) = std::max(Math::Get<1>(Model->DynamicTint), 0.0f);
	Math::Get<2>(Model->DynamicTint) = std::max(Math::Get<2>(Model->DynamicTint), 0.0f);
}

Gorc::Game::World::Level::Gameplay::ThingController& Gorc::Game::World::Level::LevelPresenter::GetThingController(Flags::ThingType type) {
	switch(type) {
	case Flags::ThingType::Actor:
		return ActorController;

	case Flags::ThingType::Cog:
		return CogController;

	case Flags::ThingType::Player:
		return PlayerController;

	case Flags::ThingType::Corpse:
		return CorpseController;

	case Flags::ThingType::Item:
		return ItemController;

	default:
	case Flags::ThingType::Ghost:
		return GhostController;
	}
}

bool Gorc::Game::World::Level::LevelPresenter::PointInsideSector(const Math::Vector<3>& position, const Gorc::Content::Assets::LevelSector& sec) {
	for(size_t i =  sec.FirstSurface; i < sec.FirstSurface + sec.SurfaceCount; ++i) {
		const auto& surf = Model->Level.Surfaces[i];
		const auto& p = Model->Level.Vertices[std::get<0>(surf.Vertices[0])];

		if(Dot(surf.Normal, position - p) < 0.0f) {
			return false;
		}
	}

	return true;
}

bool Gorc::Game::World::Level::LevelPresenter::PointPathPassesThroughAdjoin(const Math::Vector<3>& p0, const Math::Vector<3>& p1,
		const Content::Assets::LevelSector& sec, const Content::Assets::LevelSurface& surf) {
	if(Dot(p1 - p0, surf.Normal) > 0.0f) {
		return false;
	}

	auto p = Model->Level.Vertices[std::get<0>(surf.Vertices[0])];
	auto u = Dot(surf.Normal, p - p0) / Dot(surf.Normal, p1 - p0);
	if(u < 0.0f || u > 1.0f) {
		return false;
	}

	// TODO: Check for thing path passing through adjoin polygon

	return true;
}

bool Gorc::Game::World::Level::LevelPresenter::UpdatePathSector(const Vector<3>& p0, const Vector<3>& p1,
		const Content::Assets::LevelSector& sector, std::vector<std::tuple<unsigned int, unsigned int>>& path) {
	if(PointInsideSector(p1, sector)) {
		path.emplace_back(sector.Number, -1);
		return true;
	}

	for(unsigned int sec_surf_id = 0; sec_surf_id < sector.SurfaceCount; ++sec_surf_id) {
		const Content::Assets::LevelSurface& surf = Model->Surfaces[sec_surf_id + sector.FirstSurface];
		if(surf.Adjoin >= 0 && !(surf.Flags & Flags::SurfaceFlag::Impassable)
				&& PointPathPassesThroughAdjoin(p0, p1, sector, surf)) {
			path.emplace_back(sector.Number, sec_surf_id + sector.FirstSurface);

			if(UpdatePathSector(p0, p1, Model->Sectors[surf.AdjoinedSector], path)) {
				return true;
			}

			path.pop_back();
		}
	}

	return false;
}

void Gorc::Game::World::Level::LevelPresenter::UpdateThingSector(int thing_id, Thing& thing,
		const Vector<3>& oldThingPosition) {
	if(PointInsideSector(thing.Position, Model->Sectors[thing.Sector])) {
		// Thing hasn't moved to a different sector.
		return;
	}

	UpdatePathSectorScratch.clear();
	if(UpdatePathSector(oldThingPosition, thing.Position, Model->Sectors[thing.Sector], UpdatePathSectorScratch)) {
		// Fire messages along path
		unsigned int first_adjoin = std::get<1>(UpdatePathSectorScratch.front());
		if(Model->Surfaces[first_adjoin].Flags & Flags::SurfaceFlag::CogLinked) {
			ScriptPresenter.SendMessageToLinked(Cog::MessageId::Crossed, first_adjoin, Flags::MessageType::Surface,
					static_cast<int>(thing_id), Flags::MessageType::Thing);
		}

		for(unsigned int i = 1; i < UpdatePathSectorScratch.size() - 1; ++i) {
			unsigned int sec_id = std::get<0>(UpdatePathSectorScratch[i]);
			thing.Sector = sec_id;
			thing.ObjectData.SectorId = sec_id;
			if(Model->Sectors[sec_id].Flags & Flags::SectorFlag::CogLinked) {
				ScriptPresenter.SendMessageToLinked(Cog::MessageId::Entered, sec_id, Flags::MessageType::Sector,
						static_cast<int>(thing_id), Flags::MessageType::Thing);
			}

			unsigned int surf_id = std::get<1>(UpdatePathSectorScratch[i]);
			if(Model->Surfaces[surf_id].Flags & Flags::SurfaceFlag::CogLinked) {
				ScriptPresenter.SendMessageToLinked(Cog::MessageId::Crossed, surf_id, Flags::MessageType::Surface,
						static_cast<int>(thing_id), Flags::MessageType::Thing);
			}
		}

		unsigned int last_sector = std::get<0>(UpdatePathSectorScratch.back());
		thing.Sector = last_sector;
		thing.ObjectData.SectorId = last_sector;
		if(Model->Sectors[last_sector].Flags & Flags::SectorFlag::CogLinked) {
			ScriptPresenter.SendMessageToLinked(Cog::MessageId::Entered, last_sector, Flags::MessageType::Sector,
					static_cast<int>(thing_id), Flags::MessageType::Thing);
		}
	}
	else {
		// Thing hasn't moved.
		// TODO: Need to run a backup random walk here.
	}
}

void Gorc::Game::World::Level::LevelPresenter::UpdateCamera() {
	if(need_respawn) {
		DoRespawn();
	}

	Thing& camera = Model->Things[Model->CameraThingId];
	camera.Thrust = Model->CameraVelocity;
	Model->CameraVelocity = Zero<3>();

	// Update camera with eye offset
	auto p0 = camera.Position;
	auto p1 = camera.Position + camera.EyeOffset;
	if(PointInsideSector(p1, Model->Sectors[camera.Sector])) {
		// Eye offset doesn't change camera sector.
		Model->CameraSector = camera.Sector;
	}
	else {
		UpdatePathSectorScratch.clear();
		if(UpdatePathSector(p0, p1, Model->Sectors[camera.Sector], UpdatePathSectorScratch)) {
			Model->CameraSector = std::get<0>(UpdatePathSectorScratch.back());
		}
		else {
			// TODO: Back-up random walk
			Model->CameraSector = camera.Sector;
		}
	}

	Model->CameraPosition = p1;
}

void Gorc::Game::World::Level::LevelPresenter::TranslateCamera(const Vector<3>& amt) {
	Vector<3> cam_vel = Zero<3>();
	cam_vel = Zero<3>();
	cam_vel += Get<X>(amt) * Cross(Model->CameraLook, Model->CameraUp);
	cam_vel += Get<Z>(amt) * Model->CameraUp;
	cam_vel += Get<Y>(amt) * Model->CameraLook;
	cam_vel *= 0.75f;

	Get<0>(Model->CameraVelocity) = Get<0>(cam_vel);
	Get<1>(Model->CameraVelocity) = Get<1>(cam_vel);
}

void Gorc::Game::World::Level::LevelPresenter::YawCamera(double amt) {
	float sint = std::sin(amt);
	float cost = std::cos(amt);

	Vector<3> NewLook = Vec(
			cost * Get<X>(Model->CameraLook) - sint * Get<Y>(Model->CameraLook),
			sint * Get<X>(Model->CameraLook) + cost * Get<Y>(Model->CameraLook),
			Get<Z>(Model->CameraLook));
	Vector<3> NewUp = Vec(
			cost * Get<X>(Model->CameraUp) - sint * Get<Y>(Model->CameraUp),
			sint * Get<X>(Model->CameraUp) + cost * Get<Y>(Model->CameraUp),
			Get<Z>(Model->CameraUp));

	Model->CameraLook = Normalize(NewLook);
	Model->CameraUp = Normalize(NewUp);
}

void Gorc::Game::World::Level::LevelPresenter::PitchCamera(double amt) {
	float sint = std::sin(amt);
	float cost = std::cos(amt);

	Vector<3> NewUp = (-sint * Model->CameraLook) + (cost * Model->CameraUp);
	Vector<3> NewLook = (cost * Model->CameraLook) + (sint * Model->CameraUp);
	Model->CameraUp = Normalize(NewUp);
	Model->CameraLook = Normalize(NewLook);
}

void Gorc::Game::World::Level::LevelPresenter::DoRespawn() {
	need_respawn = false;

	++Model->CurrentSpawnPoint;
	Model->CurrentSpawnPoint = Model->CurrentSpawnPoint % Model->SpawnPoints.size();

	Thing& cameraThing = Model->Things[Model->CameraThingId];
	cameraThing.Sector = Model->SpawnPoints[Model->CurrentSpawnPoint]->Sector;
	cameraThing.ObjectData.SectorId = Model->SpawnPoints[Model->CurrentSpawnPoint]->Sector;
	cameraThing.Position = Model->SpawnPoints[Model->CurrentSpawnPoint]->Position;
	cameraThing.RigidBody->proceedToTransform(btTransform(btQuaternion(0,0,0,1), Math::BtVec(cameraThing.Position) * PhysicsWorldScale));
	cameraThing.RigidBody->setGravity(btVector3(0.0f, 0.0f, -Model->Header.WorldGravity * PhysicsWorldScale));
	cameraThing.AttachFlags = FlagSet<Flags::AttachFlag>();
}

void Gorc::Game::World::Level::LevelPresenter::Respawn() {
	need_respawn = true;
}

void Gorc::Game::World::Level::LevelPresenter::Jump() {
	Get<2>(Model->CameraVelocity) = 1.6f;
}

void Gorc::Game::World::Level::LevelPresenter::Activate() {
	// TODO: Implement actual surface and thing activation

	Math::Vector<3> camera_position = Model->CameraPosition;

	int best_surf_candidate = -1;
	float best_surf_dist = 0.25f;

	int best_thing_candidate = -1;
	float best_thing_dist = 0.25f;

	for(int i = 0; i < Model->Surfaces.size(); ++i) {
		const Content::Assets::LevelSurface& surf = Model->Surfaces[i];
		if((surf.Adjoin >= 0 && (Model->Adjoins[surf.Adjoin].Flags & Flags::AdjoinFlag::AllowMovement))
				|| !(surf.Flags & Flags::SurfaceFlag::CogLinked)
				|| Math::Dot(surf.Normal, Model->CameraLook) >= 0.0f) {
			continue;
		}

		for(const auto& vx : surf.Vertices) {
			float new_dist = Math::Length(camera_position - Model->Level.Vertices[std::get<0>(vx)]);
			if(new_dist < best_surf_dist) {
				best_surf_candidate = i;
				best_surf_dist = new_dist;
				break;
			}
		}
	}

	for(auto& thing : Model->Things) {
		if(thing.GetId() == Model->CameraThingId) {
			continue;
		}

		auto dir_vec = thing.Position - camera_position;
		if(!(thing.Flags & Flags::ThingFlag::CogLinked) || Math::Dot(dir_vec, Model->CameraLook) <= 0.0f) {
			continue;
		}

		float dir_len = Math::Length(dir_vec);
		if(dir_len >= best_thing_dist) {
			continue;
		}

		best_thing_candidate = thing.GetId();
		best_thing_dist = dir_len;
	}

	if(best_surf_candidate >= 0 && best_surf_dist <= best_thing_dist) {
		ScriptPresenter.SendMessageToLinked(Cog::MessageId::Activated, best_surf_candidate, Flags::MessageType::Surface,
				Model->CameraThingId, Flags::MessageType::Thing);
	}
	else if(best_thing_candidate >= 0) {
		ScriptPresenter.SendMessageToLinked(Cog::MessageId::Activated, best_thing_candidate, Flags::MessageType::Thing,
				Model->CameraThingId, Flags::MessageType::Thing);
		SoundPresenter.PlaySoundClass(best_thing_candidate, Flags::SoundSubclassType::Activate);
	}
}

void Gorc::Game::World::Level::LevelPresenter::Damage() {
	// TODO: Temporary copy of hack activate code from above.
	// Sends damaged message to surfaces and things for debugging.

	Math::Vector<3> camera_position = Model->CameraPosition;

	int best_surf_candidate = -1;
	float best_surf_dist = 0.25f;

	int best_thing_candidate = -1;
	float best_thing_dist = 0.25f;

	for(int i = 0; i < Model->Surfaces.size(); ++i) {
		const Content::Assets::LevelSurface& surf = Model->Surfaces[i];
		if((surf.Adjoin >= 0 && (Model->Adjoins[surf.Adjoin].Flags & Flags::AdjoinFlag::AllowMovement))
				|| !(surf.Flags & Flags::SurfaceFlag::CogLinked)
				|| Math::Dot(surf.Normal, Model->CameraLook) >= 0.0f) {
			continue;
		}

		for(const auto& vx : surf.Vertices) {
			float new_dist = Math::Length(camera_position - Model->Level.Vertices[std::get<0>(vx)]);
			if(new_dist < best_surf_dist) {
				best_surf_candidate = i;
				best_surf_dist = new_dist;
				break;
			}
		}
	}

	for(auto& thing : Model->Things) {
		if(thing.GetId() == Model->CameraThingId) {
			continue;
		}

		auto dir_vec = thing.Position - camera_position;
		if(!(thing.Flags & Flags::ThingFlag::CogLinked) || Math::Dot(dir_vec, Model->CameraLook) <= 0.0f) {
			continue;
		}

		float dir_len = Math::Length(dir_vec);
		if(dir_len >= best_thing_dist) {
			continue;
		}

		best_thing_candidate = thing.GetId();
		best_thing_dist = dir_len;
	}

	if(best_surf_candidate >= 0 && best_surf_dist <= best_thing_dist) {
		ScriptPresenter.SendMessageToLinked(Cog::MessageId::Damaged, best_surf_candidate, Flags::MessageType::Surface,
				Model->CameraThingId, Flags::MessageType::Thing, 1000, static_cast<int>(Flags::DamageFlag::Saber));
	}
	else if(best_thing_candidate >= 0) {
		DamageThing(best_thing_candidate, 50.0f, { Flags::DamageFlag::Saber }, Model->CameraThingId);
	}
}

void Gorc::Game::World::Level::LevelPresenter::ThingSighted(int thing_id) {
	Model->Things[thing_id].Flags += Flags::ThingFlag::Sighted;
	ScriptPresenter.SendMessageToLinked(Cog::MessageId::Sighted, thing_id, Flags::MessageType::Thing);
}

// Color verbs
void Gorc::Game::World::Level::LevelPresenter::AddDynamicTint(int player_id, const Math::Vector<3>& tint) {
	Model->DynamicTint += tint;

	// Clamp dynamic tint
	auto dynamic_tint = Model->DynamicTint;
	Math::Get<0>(Model->DynamicTint) = std::max(Math::Get<0>(dynamic_tint), 0.0f);
	Math::Get<1>(Model->DynamicTint) = std::max(Math::Get<1>(dynamic_tint), 0.0f);
	Math::Get<2>(Model->DynamicTint) = std::max(Math::Get<2>(dynamic_tint), 0.0f);
	Math::Get<0>(Model->DynamicTint) = std::min(Math::Get<0>(dynamic_tint), 1.0f);
	Math::Get<1>(Model->DynamicTint) = std::min(Math::Get<1>(dynamic_tint), 1.0f);
	Math::Get<2>(Model->DynamicTint) = std::min(Math::Get<2>(dynamic_tint), 1.0f);
}

// Frame verbs
int Gorc::Game::World::Level::LevelPresenter::GetCurFrame(int thing_id) {
	return Model->Things[thing_id].CurrentFrame;
}

void Gorc::Game::World::Level::LevelPresenter::JumpToFrame(int thing_id, int frame, int sector) {
	Thing& referenced_thing = Model->Things[thing_id];
	auto& referenced_frame = referenced_thing.Frames[frame];
	SetThingPos(thing_id, std::get<0>(referenced_frame), std::get<1>(referenced_frame), sector);
}

void Gorc::Game::World::Level::LevelPresenter::MoveToFrame(int thing_id, int frame, float speed) {
	Thing& referenced_thing = Model->Things[thing_id];

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

// Level verbs
float Gorc::Game::World::Level::LevelPresenter::GetGameTime() {
	return Model->GameTime;
}

float Gorc::Game::World::Level::LevelPresenter::GetLevelTime() {
	return Model->LevelTime;
}

// Misc verbs
void Gorc::Game::World::Level::LevelPresenter::TakeItem(int thing_id, int player_id) {
	auto& thing = Model->Things[thing_id];
	thing.Controller->Taken(thing_id, player_id);
}

// Player verbs
int Gorc::Game::World::Level::LevelPresenter::GetLocalPlayerThing() {
	return Model->CameraThingId;
}

// Sector verbs
void Gorc::Game::World::Level::LevelPresenter::SetSectorAdjoins(int sector_id, bool state) {
	Content::Assets::LevelSector& sector = Model->Sectors[sector_id];
	for(unsigned int i = 0; i < sector.SurfaceCount; ++i) {
		Content::Assets::LevelSurface& surface = Model->Surfaces[i + sector.FirstSurface];
		if(surface.Adjoin >= 0) {
			Content::Assets::LevelAdjoin& adjoin = Model->Adjoins[surface.Adjoin];

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
	Content::Assets::LevelSector& sector = Model->Sectors[sector_id];
	sector.ExtraLight = value;
}

void Gorc::Game::World::Level::LevelPresenter::SetSectorThrust(int sector_id, const Math::Vector<3>& thrust) {
	Content::Assets::LevelSector& sector = Model->Sectors[sector_id];
	sector.Thrust = thrust * RateFactor;
}

void Gorc::Game::World::Level::LevelPresenter::SetSectorTint(int sector_id, const Math::Vector<3>& color) {
	Content::Assets::LevelSector& sector = Model->Sectors[sector_id];
	sector.Tint = color;
}

// Surface verbs
void Gorc::Game::World::Level::LevelPresenter::ClearAdjoinFlags(int surface, FlagSet<Flags::AdjoinFlag> flags) {
	Content::Assets::LevelSurface& surf = Model->Surfaces[surface];
	if(surf.Adjoin >= 0) {
		Content::Assets::LevelAdjoin& adj = Model->Adjoins[surf.Adjoin];
		adj.Flags -= flags;
		Model->UpdateSurfacePhysicsProperties(surface);
	}
}

Gorc::Math::Vector<3> Gorc::Game::World::Level::LevelPresenter::GetSurfaceCenter(int surface) {
	auto vec = Math::Zero<3>();
	for(const auto& vx : Model->Level.Surfaces[surface].Vertices) {
		vec += Model->Level.Vertices[std::get<0>(vx)];
	}

	vec /= static_cast<float>(Model->Level.Surfaces[surface].Vertices.size());
	return vec;
}

void Gorc::Game::World::Level::LevelPresenter::SetAdjoinFlags(int surface, FlagSet<Flags::AdjoinFlag> flags) {
	Content::Assets::LevelSurface& surf = Model->Surfaces[surface];
	if(surf.Adjoin >= 0) {
		Content::Assets::LevelAdjoin& adj = Model->Adjoins[surf.Adjoin];
		adj.Flags += flags;
		Model->UpdateSurfacePhysicsProperties(surface);
	}
}

void Gorc::Game::World::Level::LevelPresenter::SetFaceGeoMode(int surface, Flags::GeometryMode geo_mode) {
	Model->Surfaces[surface].GeometryMode = geo_mode;
}

void Gorc::Game::World::Level::LevelPresenter::SetSurfaceFlags(int surface, FlagSet<Flags::SurfaceFlag> flags) {
	Content::Assets::LevelSurface& surf = Model->Surfaces[surface];
	surf.Flags += flags;
	Model->UpdateSurfacePhysicsProperties(surface);
}

// System verbs
int Gorc::Game::World::Level::LevelPresenter::LoadSound(const char* fn) {
	return place.ContentManager->LoadId<Content::Assets::Sound>(fn);
}

// Thing action verbs

int Gorc::Game::World::Level::LevelPresenter::CreateThing(const Content::Assets::Template& tpl, unsigned int sector_num,
		const Math::Vector<3>& pos, const Math::Vector<3>& orient) {
	// Initialize thing properties
	auto& thing = Model->Things.Create();
	thing.Reset();

	Content::Assets::Template* new_tpl = &thing;
	*new_tpl = tpl;

	thing.Sector = sector_num;
	thing.Position = pos;
	thing.ObjectData.ThingId = thing.GetId();
	thing.ObjectData.SectorId = sector_num;
	thing.Orientation = orient;
	thing.Controller = &GetThingController(thing.Type);

	thing.Controller->CreateControllerData(thing.GetId());

	if(thing.Cog) {
		ScriptPresenter.CreateGlobalCogInstance(thing.Cog->Script, *place.ContentManager, components.Compiler);
	}

	return thing.GetId();
}

int Gorc::Game::World::Level::LevelPresenter::CreateThing(int tpl_id, unsigned int sector_num,
		const Math::Vector<3>& pos, const Math::Vector<3>& orientation) {
	return CreateThing(Model->Level.Templates[tpl_id], sector_num, pos, orientation);
}

int Gorc::Game::World::Level::LevelPresenter::CreateThing(const std::string& tpl_name, unsigned int sector_num,
		const Math::Vector<3>& pos, const Math::Vector<3>& orientation) {
	std::string temp;
	std::transform(tpl_name.begin(), tpl_name.end(), std::back_inserter(temp), tolower);
	auto it = Model->Level.TemplateMap.find(temp);
	if(it != Model->Level.TemplateMap.end()) {
		return CreateThing(it->second, sector_num, pos, orientation);
	}
	else {
		// TODO: Template not found. Report error.
		return -1;
	}
}

void Gorc::Game::World::Level::LevelPresenter::AdjustThingPos(int thing_id, const Math::Vector<3>& new_pos) {
	auto& thing = Model->Things[thing_id];
	auto old_pos = thing.Position;
	thing.Position = new_pos;
	if(thing.RigidBody) {
		thing.RigidBody->getMotionState()->setWorldTransform(btTransform(thing.RigidBody->getOrientation(), Math::BtVec(new_pos) * PhysicsWorldScale));
		thing.RigidBody->proceedToTransform(btTransform(thing.RigidBody->getOrientation(), Math::BtVec(new_pos) * PhysicsWorldScale));
	}
	UpdateThingSector(thing_id, thing, old_pos);
}

void Gorc::Game::World::Level::LevelPresenter::SetThingPos(int thing_id, const Math::Vector<3>& new_pos, const Math::Vector<3>& new_orient, int new_sector) {
	Thing& thing = Model->Things[thing_id];
	thing.Position = new_pos;
	thing.Orientation = new_orient;
	thing.Sector = new_sector;

	if(thing.RigidBody) {
		btQuaternion quat(0,0,0,1);
		quat *= btQuaternion(btVector3(0,0,1), Deg2Rad * Math::Get<1>(new_orient)); // Yaw
		quat *= btQuaternion(btVector3(1,0,0), Deg2Rad * Math::Get<0>(new_orient)); // Pitch
		quat *= btQuaternion(btVector3(0,1,0), Deg2Rad * Math::Get<2>(new_orient)); // Roll
		thing.RigidBody->proceedToTransform(btTransform(quat, Math::BtVec(new_pos) * PhysicsWorldScale));
	}
}

void Gorc::Game::World::Level::LevelPresenter::AttachThingToThing(int thing_id, int base_id) {
	auto& thing = Model->Things[thing_id];
	auto& base = Model->Things[base_id];

	thing.AttachFlags = FlagSet<Flags::AttachFlag> { Flags::AttachFlag::AttachedToThing };
	thing.AttachedThing = base_id;
	thing.PrevAttachedThingPosition = base.Position;
}

int Gorc::Game::World::Level::LevelPresenter::CreateThingAtThing(int tpl_id, int thing_id) {
	Thing& referencedThing = Model->Things[thing_id];
	int new_thing_id = CreateThing(tpl_id, referencedThing.Sector, referencedThing.Position, referencedThing.Orientation);
	Thing& new_thing = Model->Things[new_thing_id];

	new_thing.PathMoving = false;

	if(new_thing.Model3d) {
		AdjustThingPos(new_thing_id, new_thing.Position + new_thing.Model3d->InsertOffset);
	}

	// CreateThingAtThing really does copy frames.
	std::transform(referencedThing.Frames.begin(), referencedThing.Frames.end(), std::back_inserter(new_thing.Frames),
			[&new_thing](const std::tuple<Vector<3>, Vector<3>>& frame) -> std::tuple<Vector<3>, Vector<3>> {
		if(new_thing.Model3d) {
			return std::make_tuple(std::get<0>(frame) + new_thing.Model3d->InsertOffset, std::get<1>(frame));
		}
		else {
			return frame;
		}
	});

	return new_thing_id;
}

float Gorc::Game::World::Level::LevelPresenter::DamageThing(int thing_id, float damage, FlagSet<Flags::DamageFlag> flags, int damager_id) {
	ScriptPresenter.SendMessageToLinked(Cog::MessageId::Damaged, static_cast<int>(thing_id), Flags::MessageType::Thing,
			damager_id, Flags::MessageType::Thing, damage, static_cast<int>(flags));

	Thing& referencedThing = Model->Things[thing_id];
	if(referencedThing.Health > 0.0f) {
		referencedThing.Health -= damage;

		if(referencedThing.Health <= 0.0f) {
			SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::Death1);
			ScriptPresenter.SendMessageToLinked(Cog::MessageId::Killed, static_cast<int>(thing_id), Flags::MessageType::Thing,
					damager_id, Flags::MessageType::Thing);
			// TODO: Thing is dead. Reset to corpse
			SetThingType(thing_id, Flags::ThingType::Corpse);
			if(referencedThing.Puppet) {
				KeyPresenter.PlayPuppetKey(thing_id, Flags::PuppetModeType::Default, Flags::PuppetSubmodeType::Death);
			}
		}
		else {
			SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::HurtSpecial);
			if(referencedThing.Puppet) {
				KeyPresenter.PlayPuppetKey(thing_id, Flags::PuppetModeType::Default, Flags::PuppetSubmodeType::Hit);
			}
		}
	}

	// TODO: Return actual 'undamaged' value as computed by thing cog with ReturnEx.

	return 0.0f;
}

void Gorc::Game::World::Level::LevelPresenter::DestroyThing(int thing_id) {
	// TODO: Clean up components owned by thing (animations, key mixes, sounds).
	Model->Things[thing_id].Controller->RemoveControllerData(thing_id);
	Model->Things.Destroy(thing_id);
}

void Gorc::Game::World::Level::LevelPresenter::DetachThing(int thing_id) {
	Model->Things[thing_id].AttachFlags = FlagSet<Flags::AttachFlag>();
}

Gorc::Math::Vector<3> Gorc::Game::World::Level::LevelPresenter::GetThingPos(int thing_id) {
	Thing& referenced_thing = Model->Things[thing_id];
	return referenced_thing.Position;
}

bool Gorc::Game::World::Level::LevelPresenter::IsThingMoving(int thing_id) {
	Thing& referencedThing = Model->Things[thing_id];
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

// Thing flags verbs
void Gorc::Game::World::Level::LevelPresenter::ClearActorFlags(int thing_id, FlagSet<Flags::ActorFlag> flags) {
	Model->Things[thing_id].ActorFlags -= flags;
}

void Gorc::Game::World::Level::LevelPresenter::ClearThingFlags(int thing_id, FlagSet<Flags::ThingFlag> flags) {
	Model->Things[thing_id].Flags -= flags;
}

Gorc::FlagSet<Gorc::Flags::ActorFlag> Gorc::Game::World::Level::LevelPresenter::GetActorFlags(int thing_id) {
	return Model->Things[thing_id].ActorFlags;
}

void Gorc::Game::World::Level::LevelPresenter::SetActorFlags(int thing_id, FlagSet<Flags::ActorFlag> flags) {
	Model->Things[thing_id].ActorFlags += flags;
}

// Thing property verbs
int Gorc::Game::World::Level::LevelPresenter::GetThingParent(int thing_id) {
	return Model->Things[thing_id].AttachedThing;
}

int Gorc::Game::World::Level::LevelPresenter::GetThingSector(int thing_id) {
	return Model->Things[thing_id].Sector;
}

Gorc::Flags::ThingType Gorc::Game::World::Level::LevelPresenter::GetThingType(int thing_id) {
	return Model->Things[thing_id].Type;
}

void Gorc::Game::World::Level::LevelPresenter::SetThingType(int thing_id, Flags::ThingType type) {
	// Clean up type physics.
	auto& thing = Model->Things[thing_id];
	thing.Controller->RemoveControllerData(thing_id);

	thing.Type = type;

	// Install new controller
	thing.Controller = &GetThingController(thing.Type);
	thing.Controller->CreateControllerData(thing_id);
}

void Gorc::Game::World::Level::LevelPresenter::SetThingLight(int thing_id, float light, float fade_time) {
	// TODO: Implement fade_time
	auto& thing = Model->Things[thing_id];
	thing.Light = light;
}

void Gorc::Game::World::Level::LevelPresenter::RegisterVerbs(Cog::Verbs::VerbTable& verbTable, Components& components) {
	Animations::AnimationPresenter::RegisterVerbs(verbTable, components);
	Scripts::ScriptPresenter::RegisterVerbs(verbTable, components);
	Sounds::SoundPresenter::RegisterVerbs(verbTable, components);
	Keys::KeyPresenter::RegisterVerbs(verbTable, components);
	Gameplay::InventoryPresenter::RegisterVerbs(verbTable, components);

	// Color verbs
	verbTable.AddVerb<void, 4>("adddynamictint", [&components](int player_id, float r, float g, float b) {
		components.CurrentLevelPresenter->AddDynamicTint(player_id, Math::Vec(r, g, b));
	});

	// Frame verbs
	verbTable.AddVerb<int, 1>("getcurframe", [&components](int thing) {
		return components.CurrentLevelPresenter->GetCurFrame(thing);
	});

	verbTable.AddVerb<void, 3>("jumptoframe", [&components](int thing, int frame, int sector) {
		return components.CurrentLevelPresenter->JumpToFrame(thing, frame, sector);
	});

	verbTable.AddVerb<void, 3>("movetoframe", [&components](int thing, int frame, float speed) {
		return components.CurrentLevelPresenter->MoveToFrame(thing, frame, speed);
	});

	// Level verbs
	verbTable.AddVerb<float, 0>("getgametime", [&components] {
		return components.CurrentLevelPresenter->GetGameTime();
	});

	verbTable.AddVerb<float, 0>("getleveltime", [&components] {
		return components.CurrentLevelPresenter->GetLevelTime();
	});

	// Misc verbs
	verbTable.AddVerb<void, 2>("takeitem", [&components](int thing_id, int player_id) {
		components.CurrentLevelPresenter->TakeItem(thing_id, player_id);
	});

	// Options verbs
	verbTable.AddVerb<int, 0>("getdifficulty", [] {
		// TODO: Add actual difficulty setting.
		return static_cast<int>(Flags::DifficultyMode::Medium);
	});

	verbTable.AddVerb<int, 0>("ismulti", [] {
		// TODO: Return actual multiplayer state.
		return 0;
	});

	// Player verbs
	verbTable.AddVerb<int, 0>("getlocalplayerthing", [&components] {
		return components.CurrentLevelPresenter->GetLocalPlayerThing();
	});

	verbTable.AddVerb<int, 0>("jkgetlocalplayer", [&components] {
		return components.CurrentLevelPresenter->GetLocalPlayerThing();
	});

	// Print verbs
	verbTable.AddVerb<void, 2>("jkprintunistring", [&components](int destination, int message_num) {
		// TODO: Add actual jkPrintUniString once localization is implemented.
		std::cout << "COG_" << message_num << std::endl;
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

	verbTable.AddVerb<void, 2>("setfacegeomode", [&components](int surface, int mode) {
		components.CurrentLevelPresenter->SetFaceGeoMode(surface, static_cast<Flags::GeometryMode>(mode));
	});

	verbTable.AddVerb<void, 2>("setsurfaceflags", [&components](int surface, int flags) {
		components.CurrentLevelPresenter->SetSurfaceFlags(surface, FlagSet<Flags::SurfaceFlag>(flags));
	});

	verbTable.AddVerb<Math::Vector<3>, 1>("surfacecenter", [&components](int surface) {
		return components.CurrentLevelPresenter->GetSurfaceCenter(surface);
	});

	// System verbs
	verbTable.AddVerb<int, 2>("bittest", [](int flag1, int flag2) {
		return flag1 & flag2;
	});

	verbTable.AddVerb<int, 1>("loadsound", [&components](const char* fn) {
		return components.CurrentLevelPresenter->LoadSound(fn);
	});

	verbTable.AddVerb<float, 0>("rand", []{ return sf::Randomizer::Random(0.0f, 1.0f); });

	// Thing action verbs
	verbTable.AddVerb<void, 2>("attachthingtothing", [&components](int attach_thing, int base_thing) {
		components.CurrentLevelPresenter->AttachThingToThing(attach_thing, base_thing);
	});

	verbTable.AddVerb<int, 2>("creatething", [&components](int tpl_id, int thing_pos) {
		return components.CurrentLevelPresenter->CreateThingAtThing(tpl_id, thing_pos);
	});

	verbTable.AddVerb<float, 4>("damagething", [&components](int thing_id, float damage, int flags, int damager_id) {
		return components.CurrentLevelPresenter->DamageThing(thing_id, damage, FlagSet<Flags::DamageFlag>(flags), damager_id);
	});

	verbTable.AddVerb<void, 1>("destroything", [&components](int thing_id) {
		components.CurrentLevelPresenter->DestroyThing(thing_id);
	});

	verbTable.AddVerb<void, 1>("detachthing", [&components](int thing_id) {
		components.CurrentLevelPresenter->DetachThing(thing_id);
	});

	verbTable.AddVerb<Math::Vector<3>, 1>("getthingpos", [&components](int thing_id) {
		return components.CurrentLevelPresenter->GetThingPos(thing_id);
	});

	verbTable.AddVerb<bool, 1>("isthingmoving", [&components](int thing_id) {
		return components.CurrentLevelPresenter->IsThingMoving(thing_id);
	});

	verbTable.AddVerb<bool, 1>("ismoving", [&components](int thing_id) {
		return components.CurrentLevelPresenter->IsThingMoving(thing_id);
	});

	// Thing flags verbs
	verbTable.AddVerb<void, 2>("clearactorflags", [&components](int thing_id, int flags) {
		components.CurrentLevelPresenter->ClearActorFlags(thing_id, FlagSet<Flags::ActorFlag>(flags));
	});

	verbTable.AddVerb<void, 2>("clearthingflags", [&components](int thing_id, int flags) {
		components.CurrentLevelPresenter->ClearThingFlags(thing_id, FlagSet<Flags::ThingFlag>(flags));
	});

	verbTable.AddVerb<int, 1>("getactorflags", [&components](int thing_id) {
		return static_cast<int>(components.CurrentLevelPresenter->GetActorFlags(thing_id));
	});

	verbTable.AddVerb<void, 2>("setactorflags", [&components](int thing_id, int flags) {
		components.CurrentLevelPresenter->SetActorFlags(thing_id, FlagSet<Flags::ActorFlag>(flags));
	});

	// Thing property verbs
	verbTable.AddVerb<int, 1>("getthingparent", [&components](int thing_id) {
		return components.CurrentLevelPresenter->GetThingParent(thing_id);
	});

	verbTable.AddVerb<int, 1>("getthingsector", [&components](int thing_id) {
		return components.CurrentLevelPresenter->GetThingSector(thing_id);
	});

	verbTable.AddVerb<int, 1>("getthingtype", [&components](int thing_id) {
		return static_cast<int>(components.CurrentLevelPresenter->GetThingType(thing_id));
	});

	verbTable.AddVerb<void, 3>("setthinglight", [&components](int thing_id, float light, float fade_time) {
		components.CurrentLevelPresenter->SetThingLight(thing_id, light, fade_time);
	});

	verbTable.AddVerb<void, 3>("thinglight", [&components](int thing_id, float light, float fade_time) {
		components.CurrentLevelPresenter->SetThingLight(thing_id, light, fade_time);
	});
}
