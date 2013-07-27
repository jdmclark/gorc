#include "levelmodel.h"
#include "game/world/level/physics/physicscollideclass.h"
#include "framework/flagset.h"
#include "content/manager.h"

Gorc::Game::World::Level::LevelModel::LevelModel(Gorc::Content::Manager& ContentManager, Cog::Compiler& CogCompiler, const Gorc::Content::Assets::Level& Level)
	: Level(Level), Header(Level.Header), Adjoins(Level.Adjoins), Surfaces(Level.Surfaces), Sectors(Level.Sectors),
	  Dispatcher(&CollisionConfiguration), DynamicsWorld(&Dispatcher, &Broadphase, &ConstraintSolver, &CollisionConfiguration),
	  SurfaceMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0))), SurfaceObjectData(Level.Surfaces.size()) {
	BroadphaseFilter = std::unique_ptr<SectorBroadphaseFilter>(new SectorBroadphaseFilter(*this));
	DynamicsWorld.getPairCache()->setOverlapFilterCallback(BroadphaseFilter.get());
	DynamicsWorld.setGravity(btVector3(0, 0, -Level.Header.WorldGravity));

	// Construct surface rigid bodies.
	for(const auto& sec : Level.Sectors) {
		for(size_t i = sec.FirstSurface; i < sec.FirstSurface + sec.SurfaceCount; ++i) {
			SurfaceObjectData[i].SurfaceId = i;
			SurfaceObjectData[i].SectorId = sec.Number;
		}
	}

	for(size_t i = 0; i < Level.Surfaces.size(); ++i) {
		const auto& surf = Level.Surfaces[i];

		btRigidBody::btRigidBodyConstructionInfo surf_ci(0, &SurfaceMotionState,
				const_cast<btConvexHullShape*>(Level.SurfaceShapes[i].get()),
				btVector3(0,0,0));
		if(!(surf.Flags & Flags::SurfaceFlag::Floor)) {
			surf_ci.m_friction = 0.0f;
		}

		SurfaceRigidBodies.emplace_back(new btRigidBody(surf_ci));
		SurfaceRigidBodies[i]->setUserPointer(&SurfaceObjectData[i]);

		DynamicsWorld.addRigidBody(SurfaceRigidBodies.back().get());
		UpdateSurfacePhysicsProperties(i, true);
	}

	return;
}

unsigned int Gorc::Game::World::Level::LevelModel::CreateThing(const Content::Assets::Template& tpl, unsigned int sector_num,
		const Math::Vector<3>& pos, const Math::Vector<3>& orient) {
	if(tpl.Flags & Flags::ThingFlag::NotInEasy) {
		// TODO: Temporarily force the game to easy mode.
		// Insert ghost thing to fill ID.
		auto new_thing_tuple = Things.Create();
		auto& new_thing = *std::get<0>(new_thing_tuple);
		new_thing.Position = pos;
		new_thing.Orientation = orient;
		new_thing.Type = Flags::ThingType::Ghost;
		return std::get<1>(new_thing_tuple);
	}
	else if(tpl.Type == Flags::ThingType::Actor || tpl.Type == Flags::ThingType::Player) {
		auto new_thing_tuple = Things.Create();
		auto& new_thing = *std::get<0>(new_thing_tuple);
		new_thing = tpl;

		new_thing.Sector = sector_num;
		new_thing.Position = pos;
		new_thing.Orientation = orient;

		static const float deg2rad = 0.0174532925f;
		btQuaternion orientation(0.0f, 0.0f, 0.0f, 1.0f);
		orientation *= btQuaternion(btVector3(0,0,1), deg2rad * Math::Get<1>(orient)); // Yaw
		orientation *= btQuaternion(btVector3(1,0,0), deg2rad * Math::Get<0>(orient)); // Pitch
		orientation *= btQuaternion(btVector3(0,1,0), deg2rad * Math::Get<2>(orient)); // Roll

		float thing_mass = 0.0f;
		if(new_thing.Move == Flags::MoveType::Physics && new_thing.Collide != Flags::CollideType::None) {
			thing_mass = new_thing.Mass;
		}

		btVector3 positions[] {
			btVector3(0.0f, 0.0f, 0.0f),
			btVector3(0.0f, 0.0f, -0.02f -new_thing.Height * 0.5f)
		};

		btScalar radii[] {
			0.04f, 0.02f
		};

		new_thing.ActorCollideShape = std::unique_ptr<btCollisionShape>(new btMultiSphereShape(positions, radii, 2));
		btCollisionShape* thingShape = new_thing.ActorCollideShape.get();

		btVector3 thing_inertia(0,0,0);
		thingShape->calculateLocalInertia(thing_mass, thing_inertia);

		new_thing.MotionState = std::unique_ptr<btDefaultMotionState>(new btDefaultMotionState(
				btTransform(orientation, Math::BtVec(pos))));

		btRigidBody::btRigidBodyConstructionInfo actor_ci(thing_mass, new_thing.MotionState.get(),
				thingShape, thing_inertia);
		actor_ci.m_friction = 2.0f;
		new_thing.RigidBody = std::unique_ptr<btRigidBody>(new btRigidBody(actor_ci));

		FlagSet<PhysicsCollideClass> CollideType {PhysicsCollideClass::Thing};

		if(tpl.Type == Flags::ThingType::Player) {
			CollideType += PhysicsCollideClass::Player;
		}
		else {
			CollideType += PhysicsCollideClass::Enemy;
		}

		FlagSet<PhysicsCollideClass> CollideWith;

		if(new_thing.Collide != Flags::CollideType::None) {
			CollideWith = {PhysicsCollideClass::Wall, PhysicsCollideClass::Adjoin, PhysicsCollideClass::Thing};
		}

		// Associate thing info structure.
		new_thing.ObjectData.ThingId = std::get<1>(new_thing_tuple);
		new_thing.ObjectData.SectorId = sector_num;
		new_thing.ObjectData.CollisionGroup = CollideType;
		new_thing.ObjectData.CollisionMask = CollideWith;
		new_thing.RigidBody->setUserPointer(&new_thing.ObjectData);

		if(new_thing.Move == Flags::MoveType::Path && new_thing.Frames.size() > 0) {
			new_thing.RigidBody->setCollisionFlags(new_thing.RigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			new_thing.RigidBody->setActivationState(ISLAND_SLEEPING);
		}
		else {
			new_thing.RigidBody->setActivationState(ISLAND_SLEEPING);
		}

		new_thing.RigidBody->setAngularFactor(btVector3(0,0,0));

		DynamicsWorld.addRigidBody(new_thing.RigidBody.get());

		// HACK: Set running animation as the currently playing animation if the actor has a pup.
		if(new_thing.Puppet) {
			const Content::Assets::PuppetSubmode& run_mode =
					new_thing.Puppet->GetMode(Flags::PuppetModeType::Default).GetSubmode(Flags::PuppetSubmodeType::Stand);
			if(run_mode.Animation) {
				new_thing.CurrentPlayingAnimation = run_mode.Animation;
			}
		}

		return std::get<1>(new_thing_tuple);
	}
	else if(tpl.Model3d) {
		auto new_thing_tuple = Things.Create();
		auto& new_thing = *std::get<0>(new_thing_tuple);
		new_thing = tpl;

		new_thing.Sector = sector_num;
		new_thing.Position = pos;
		new_thing.Orientation = orient;

		static const float deg2rad = 0.0174532925f;
		btQuaternion orientation(0.0f, 0.0f, 0.0f, 1.0f);
		orientation *= btQuaternion(btVector3(0,0,1), deg2rad * Math::Get<1>(orient)); // Yaw
		orientation *= btQuaternion(btVector3(1,0,0), deg2rad * Math::Get<0>(orient)); // Pitch
		orientation *= btQuaternion(btVector3(0,1,0), deg2rad * Math::Get<2>(orient)); // Roll

		float thing_mass = 0.0f;
		if(new_thing.Move == Flags::MoveType::Physics && new_thing.Collide != Flags::CollideType::None) {
			thing_mass = new_thing.Mass;
		}

		btCollisionShape* thingShape;
		if(tpl.Flags & FlagSet<Flags::ThingFlag>{
			Flags::ThingFlag::PartOfWorldGeometry,
			Flags::ThingFlag::CanStandOn }) {
			thingShape = new_thing.Model3d->Shape.get();
		}
		else {
			new_thing.ActorCollideShape = std::unique_ptr<btCollisionShape>(new btSphereShape(tpl.Size * 0.5f));
			thingShape = new_thing.ActorCollideShape.get();
		}

		btVector3 thing_inertia(0,0,0);
		thingShape->calculateLocalInertia(thing_mass, thing_inertia);

		new_thing.MotionState = std::unique_ptr<btDefaultMotionState>(new btDefaultMotionState(
				btTransform(orientation, Math::BtVec(pos))));
		new_thing.RigidBody = std::unique_ptr<btRigidBody>(new btRigidBody(
				btRigidBody::btRigidBodyConstructionInfo(thing_mass, new_thing.MotionState.get(),
						thingShape, thing_inertia)));

		FlagSet<PhysicsCollideClass> CollideType {PhysicsCollideClass::Thing};
		FlagSet<PhysicsCollideClass> CollideWith;

		if(new_thing.Collide != Flags::CollideType::None) {
			CollideWith = {PhysicsCollideClass::Wall, PhysicsCollideClass::Adjoin, PhysicsCollideClass::Thing};
		}

		// Associate thing info structure.
		new_thing.ObjectData.ThingId = std::get<1>(new_thing_tuple);
		new_thing.ObjectData.SectorId = sector_num;
		new_thing.ObjectData.CollisionGroup = CollideType;
		new_thing.ObjectData.CollisionMask = CollideWith;

		new_thing.RigidBody->setUserPointer(&new_thing.ObjectData);

		if(new_thing.Move == Flags::MoveType::Path && new_thing.Frames.size() > 0) {
			new_thing.RigidBody->setCollisionFlags(new_thing.RigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			new_thing.RigidBody->setActivationState(ISLAND_SLEEPING);
		}
		else {
			new_thing.RigidBody->setActivationState(ISLAND_SLEEPING);
		}

		DynamicsWorld.addRigidBody(new_thing.RigidBody.get());
		return std::get<1>(new_thing_tuple);
	}
	else {
		// TODO: Add ghostdecor thing.
		// For now, insert empty thing to fill ID.
		auto new_thing_tuple = Things.Create();
		auto& new_thing = *std::get<0>(new_thing_tuple);
		new_thing.Type = Flags::ThingType::Ghost;
		new_thing.Position = pos;
		new_thing.Orientation = orient;
		return std::get<1>(new_thing_tuple);
	}
}

unsigned int Gorc::Game::World::Level::LevelModel::CreateThing(int tpl_id, unsigned int sector_num,
		const Math::Vector<3>& pos, const Math::Vector<3>& orientation) {
	return CreateThing(Level.Templates[tpl_id], sector_num, pos, orientation);
}

unsigned int Gorc::Game::World::Level::LevelModel::CreateThing(const std::string& tpl_name, unsigned int sector_num,
		const Math::Vector<3>& pos, const Math::Vector<3>& orientation) {
	std::string temp;
	std::transform(tpl_name.begin(), tpl_name.end(), std::back_inserter(temp), tolower);
	auto it = Level.TemplateMap.find(temp);
	if(it != Level.TemplateMap.end()) {
		return CreateThing(it->second, sector_num, pos, orientation);
	}
	else {
		// TODO: Template not found. Report error.
		return -1;
	}
}

void Gorc::Game::World::Level::LevelModel::UpdateSurfacePhysicsProperties(int surface, bool initial) {
	auto& surf = Surfaces[surface];
	if(surf.Adjoin >= 0 && (Adjoins[surf.Adjoin].Flags & Flags::AdjoinFlag::AllowMovement)) {
		// I guess, apparently, if you set the passable flag for an adjoin, the game
		// automatically clears the impassable flag on the surface?
		surf.Flags -= Flags::SurfaceFlag::Impassable;
	}

	FlagSet<PhysicsCollideClass> CollideType;
	if(surf.Adjoin >= 0) {
		CollideType += PhysicsCollideClass::Adjoin;
	}
	else {
		CollideType += PhysicsCollideClass::Wall;
	}

	FlagSet<PhysicsCollideClass> CollidesWith;
	if(surf.Adjoin < 0 || (surf.Flags & Flags::SurfaceFlag::Impassable) ||
			!(Adjoins[surf.Adjoin].Flags & Flags::AdjoinFlag::AllowMovement)) {
		CollidesWith += PhysicsCollideClass::Thing;
	}

	if(surf.Adjoin >= 0 && (Adjoins[surf.Adjoin].Flags & Flags::AdjoinFlag::AllowAiOnly)) {
		CollidesWith += PhysicsCollideClass::Player;
	}

	if(surf.Adjoin >= 0 && (Adjoins[surf.Adjoin].Flags & Flags::AdjoinFlag::AllowPlayerOnly)) {
		CollidesWith += PhysicsCollideClass::Enemy;
	}

	auto& physics_data = SurfaceObjectData[surface];
	physics_data.CollisionGroup = CollideType;
	physics_data.CollisionMask = CollidesWith;

	if(!initial) {
		// TODO: Just remove relevant manifolds.
		DynamicsWorld.removeRigidBody(SurfaceRigidBodies[surface].get());
		DynamicsWorld.addRigidBody(SurfaceRigidBodies[surface].get());
	}
}
