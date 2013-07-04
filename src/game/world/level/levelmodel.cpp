#include "levelmodel.h"
#include "framework/flagset.h"

Gorc::Game::World::Level::PhysicsObjectData::~PhysicsObjectData() {
	return;
}

Gorc::Game::World::Level::Thing::Thing(const Content::Assets::Template& tpl)
	: Content::Assets::Template(tpl) {
	return;
}

const Gorc::Content::Assets::Template& Gorc::Game::World::Level::Thing::operator=(const Content::Assets::Template& tpl) {
	return Template::operator=(tpl);
}

Gorc::Game::World::Level::TimerEntity::~TimerEntity() {
	return;
}

Gorc::Game::World::Level::SurfaceAnimEntity::SurfaceAnimEntity(LevelModel& model, unsigned int surface, double framerate,
		FlagSet<Content::Assets::SurfaceAnimationFlag> flag)
	: model(model), surface(surface), framerate(1.0 / framerate), flag(flag), framerate_accumulator(0.0) {

	int surface_material = model.Level.Surfaces[surface].Material;
	if(surface_material < 0) {
		// TODO: Surface has no material but has an animation? Report error.
		num_cels = 0;
		return;
	}

	num_cels = model.Level.Materials[surface_material]->Cels.size();

	if(flag & Content::Assets::SurfaceAnimationFlag::SkipFirstTwoFrames) {
		model.SurfaceCelNumber[surface] = 2 % num_cels;
	}
	else if(flag & Content::Assets::SurfaceAnimationFlag::SkipFirstFrame) {
		model.SurfaceCelNumber[surface] = 1 % num_cels;
	}
	else {
		model.SurfaceCelNumber[surface] = 0;
	}
}

void Gorc::Game::World::Level::SurfaceAnimEntity::Update(double dt) {
	framerate_accumulator += dt;

	while(framerate_accumulator >= framerate) {
		framerate_accumulator -= framerate;

		int next_cel = model.SurfaceCelNumber[surface] + 1;
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

		model.SurfaceCelNumber[surface] = next_cel;
	}
}

Gorc::Game::World::Level::LevelModel::LevelModel(const Gorc::Content::Assets::Level& Level)
	: Level(Level), MaterialCelNumber(Level.MaterialEntries.size(), 0), SurfaceCelNumber(Level.Surfaces.size(), -1), SurfaceAnimNumber(Level.Surfaces.size(), -1),
	  Dispatcher(&CollisionConfiguration), DynamicsWorld(&Dispatcher, &Broadphase, &ConstraintSolver, &CollisionConfiguration),
	  SurfaceMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0))), SurfaceObjectData(Level.Surfaces.size()) {
	DynamicsWorld.setGravity(btVector3(0, 0, -Level.Header.WorldGravity));

	// Construct surface rigid bodies.
	size_t i = 0;
	for(const auto& surf : Level.Surfaces) {
		SurfaceRigidBodies.emplace_back(new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(
				0, &SurfaceMotionState, const_cast<btConvexHullShape*>(&surf.SurfaceShape), btVector3(0,0,0))));

		FlagSet<PhysicsCollideClass> CollideType;
		if(surf.Adjoin >= 0) {
			CollideType += PhysicsCollideClass::Adjoin;
		}
		else {
			CollideType += PhysicsCollideClass::Wall;
		}

		FlagSet<PhysicsCollideClass> CollidesWith;
		if(surf.Adjoin < 0 || (surf.Flags & Content::Assets::SurfaceFlag::Impassable) ||
				!(Level.Adjoins[surf.Adjoin].Flags & Content::Assets::SurfaceAdjoinFlag::AllowMovement)) {
			CollidesWith += PhysicsCollideClass::Thing;
		}

		if(surf.Adjoin >= 0 && (Level.Adjoins[surf.Adjoin].Flags & Content::Assets::SurfaceAdjoinFlag::AllowAiOnly)) {
			CollidesWith += PhysicsCollideClass::Player;
		}

		if(surf.Adjoin >= 0 && (Level.Adjoins[surf.Adjoin].Flags & Content::Assets::SurfaceAdjoinFlag::AllowPlayerOnly)) {
			CollidesWith += PhysicsCollideClass::Enemy;
		}

		DynamicsWorld.addRigidBody(SurfaceRigidBodies.back().get(), static_cast<unsigned int>(CollideType), static_cast<unsigned int>(CollidesWith));
	}

	for(const auto& sec : Level.Sectors) {
		for(size_t i = sec.FirstSurface; i < sec.FirstSurface + sec.SurfaceCount; ++i) {
			SurfaceObjectData[i].SurfaceId = i;
			SurfaceObjectData[i].SectorId = sec.Number;
		}
	}

	for(size_t i = 0; i < Level.Surfaces.size(); ++i) {
		SurfaceRigidBodies[i]->setUserPointer(&SurfaceObjectData[i]);
	}

	// HACK: Create thing collision shapes and rigid bodies, enumerate spawn points
	for(const auto& thing : Level.Things) {
		CreateThing(thing, thing.Sector, thing.Position, thing.Orientation);
	}

	// HACK: Spawn camera thing
	CurrentSpawnPoint = 0;
	auto camera_thing_tuple = Things.Create();
	auto& camera_thing = *std::get<0>(camera_thing_tuple);
	CameraThingId = std::get<1>(camera_thing_tuple);

	CameraShape = std::unique_ptr<btSphereShape>(new btSphereShape(CameraRadius));

	camera_thing.Type = Content::Assets::ThingType::Player;
	camera_thing.Sector = SpawnPoints[CurrentSpawnPoint]->Sector;
	camera_thing.Position = SpawnPoints[CurrentSpawnPoint]->Position;

	btScalar cameraMass = SpawnPoints[CurrentSpawnPoint]->Mass;
	btVector3 cameraFallInertia(0,0,0);
	CameraShape->calculateLocalInertia(cameraMass, cameraFallInertia);

	camera_thing.MotionState = std::unique_ptr<btDefaultMotionState>(new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), BtVec(camera_thing.Position))));
	camera_thing.RigidBody = std::unique_ptr<btRigidBody>(new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(cameraMass,
			camera_thing.MotionState.get(), CameraShape.get(), cameraFallInertia)));
	camera_thing.RigidBody->setFlags(BT_DISABLE_WORLD_GRAVITY);
	camera_thing.RigidBody->setSleepingThresholds(0, 0);

	camera_thing.ObjectData.ThingId = CameraThingId;
	camera_thing.RigidBody->setUserPointer(&camera_thing.ObjectData);

	FlagSet<PhysicsCollideClass> CameraCollideType {PhysicsCollideClass::Player, PhysicsCollideClass::Thing};
	FlagSet<PhysicsCollideClass> CameraCollideWith {PhysicsCollideClass::Wall, PhysicsCollideClass::Adjoin, PhysicsCollideClass::Thing};

	DynamicsWorld.addRigidBody(camera_thing.RigidBody.get(), static_cast<unsigned int>(CameraCollideType),
			static_cast<unsigned int>(CameraCollideWith));

	Broadphase.optimize();

	// Create COG script instances.
	for(const auto& cog : Level.Cogs) {
		Content::Assets::Script const* script = std::get<0>(cog);
		const std::vector<Cog::VM::Value>& values = std::get<1>(cog);

		Cogs.push_back(script->Script.CreateInstance(values));
	}

	return;
}

unsigned int Gorc::Game::World::Level::LevelModel::CreateThing(const Content::Assets::Template& tpl, unsigned int sector_num,
		const Math::Vector<3>& pos, const Math::Vector<3>& orient) {
	if(tpl.Type == Content::Assets::ThingType::Player) {
		// Add spawn point to spawn point list.
		SpawnPoints.push_back(&tpl);

		// Insert ghost thing to fill ID.
		auto new_thing_tuple = Things.Create();
		auto& new_thing = *std::get<0>(new_thing_tuple);
		new_thing.Type = Content::Assets::ThingType::Ghost;
		return std::get<1>(new_thing_tuple);
	}
	else if(tpl.Type == Content::Assets::ThingType::Actor) {
		// TODO: Add actual enemy thing.
		// For now, insert ghost thing to fill ID.
		auto new_thing_tuple = Things.Create();
		auto& new_thing = *std::get<0>(new_thing_tuple);
		new_thing.Type = Content::Assets::ThingType::Ghost;
		return std::get<1>(new_thing_tuple);
	}
	else if(tpl.Model3d) {
		auto new_thing_tuple = Things.Create();
		auto& new_thing = *std::get<0>(new_thing_tuple);
		new_thing = tpl;

		new_thing.Sector = sector_num;

		static const float deg2rad = 0.0174532925f;
		btQuaternion orientation = btQuaternion(btVector3(1.0f, 0.0f, 0.0f), deg2rad * Math::Get<0>(orient))
				* btQuaternion(btVector3(0.0f, 0.0f, 1.0f), deg2rad * Math::Get<1>(orient))
				* btQuaternion(btVector3(0.0f, 1.0f, 0.0f), deg2rad * Math::Get<2>(orient));

		float thing_mass = 0.0f;
		if(new_thing.Move == Content::Assets::MoveType::Physics && new_thing.Collide != Content::Assets::CollideType::None) {
			thing_mass = new_thing.Mass;
		}

		btCollisionShape* thingShape = new_thing.Model3d->Shape.get();

		btVector3 thing_inertia(0,0,0);
		thingShape->calculateLocalInertia(thing_mass, thing_inertia);

		new_thing.MotionState = std::unique_ptr<btDefaultMotionState>(new btDefaultMotionState(
				btTransform(orientation, Math::BtVec(pos))));
		new_thing.RigidBody = std::unique_ptr<btRigidBody>(new btRigidBody(
				btRigidBody::btRigidBodyConstructionInfo(thing_mass, new_thing.MotionState.get(),
						thingShape, thing_inertia)));

		FlagSet<PhysicsCollideClass> CollideType {PhysicsCollideClass::Thing};
		FlagSet<PhysicsCollideClass> CollideWith;

		if(new_thing.Collide != Content::Assets::CollideType::None) {
			CollideWith = {PhysicsCollideClass::Wall, PhysicsCollideClass::Adjoin, PhysicsCollideClass::Thing};
		}

		DynamicsWorld.addRigidBody(new_thing.RigidBody.get(), static_cast<unsigned int>(CollideType),
				static_cast<unsigned int>(CollideWith));

		// Associate thing info structure.
		new_thing.ObjectData.ThingId = std::get<1>(new_thing_tuple);
		new_thing.RigidBody->setUserPointer(&new_thing.ObjectData);

		new_thing.RigidBody->setActivationState(ISLAND_SLEEPING);

		return std::get<1>(new_thing_tuple);
	}
	else {
		// TODO: Add ghostdecor thing.
		// For now, insert empty thing to fill ID.
		auto new_thing_tuple = Things.Create();
		auto& new_thing = *std::get<0>(new_thing_tuple);
		new_thing.Type = Content::Assets::ThingType::Ghost;
		return std::get<1>(new_thing_tuple);
	}
}

unsigned int Gorc::Game::World::Level::LevelModel::CreateThing(const std::string& tpl_name, unsigned int sector_num,
		const Math::Vector<3>& pos, const Math::Vector<3>& orientation) {
	std::string temp;
	std::transform(tpl_name.begin(), tpl_name.end(), std::back_inserter(temp), tolower);
	auto it = Level.Templates.find(temp);
	if(it != Level.Templates.end()) {
		return CreateThing(it->second, sector_num, pos, orientation);
	}
	else {
		// TODO: Template not found. Report error.
		return -1;
	}
}
