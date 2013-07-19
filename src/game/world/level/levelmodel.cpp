#include "levelmodel.h"
#include "physicscollideclass.h"
#include "framework/flagset.h"
#include "content/manager.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

void OnAnimationDestroy(Pool<std::unique_ptr<Animation>>& pool, unsigned int index, std::unique_ptr<Animation>& anim) {
	anim->Stop();
}

}
}
}
}

void Gorc::Game::World::Level::LevelModel::AddCogInstance(const Cog::Script& script, Content::Manager& manager, Cog::Compiler& compiler,
		const std::vector<Cog::VM::Value>& values) {
	Cogs.emplace_back(std::unique_ptr<Cog::Instance>(new Cog::Instance(script)), CogTimerState());
	Cog::Instance& inst = *std::get<0>(Cogs.back());

	inst.Heap.resize(script.SymbolTable.size());

	auto it = script.SymbolTable.begin();
	auto jt = inst.Heap.begin();
	auto kt = values.begin();

	for( ; it != script.SymbolTable.end() && jt != inst.Heap.end(); ++it, ++jt) {
		if(kt != values.end() && !it->Local && it->Type != Cog::Symbols::SymbolType::Message) {
			(*jt) = *kt;
			++kt;
		}
		else {
			(*jt) = it->DefaultValue;
		}
	}

	// Load instance resources and set flags
	it = script.SymbolTable.begin();
	jt = inst.Heap.begin();

	for( ; it != script.SymbolTable.end() && jt != inst.Heap.end(); ++it, ++jt) {
		switch(it->Type) {
		case Cog::Symbols::SymbolType::Cog:
			try {
				(*jt) = manager.LoadId<Content::Assets::Script>(static_cast<const char*>(*jt), compiler);
			}
			catch(...) {
				(*jt) = nullptr;
			}
			break;

		case Cog::Symbols::SymbolType::Material:
			try {
				(*jt) = manager.LoadId<Content::Assets::Material>(static_cast<const char*>(*jt), *Level.MasterColormap);
			}
			catch(...) {
				(*jt) = nullptr;
			}
			break;

		case Cog::Symbols::SymbolType::Model:
			try {
				(*jt) = manager.LoadId<Content::Assets::Model>(static_cast<const char*>(*jt), *Level.MasterColormap);
			}
			catch(...) {
				(*jt) = nullptr;
			}
			break;

		case Cog::Symbols::SymbolType::Sound:
			try {
				(*jt) = manager.LoadId<Content::Assets::Sound>(static_cast<const char*>(*jt));
			}
			catch(...) {
				(*jt) = nullptr;
			}
			break;

		case Cog::Symbols::SymbolType::Template: {
			auto it = Level.TemplateMap.find(static_cast<const char*>(*jt));
			if(it == Level.TemplateMap.end()) {
				// TODO: Template not found, report error.
				(*jt) = -1;
			}
			else {
				(*jt) = it->second;
			}
		}
		break;

		case Cog::Symbols::SymbolType::Sector: {
			int index = static_cast<int>(*jt);
			if(index >= 0) {
				Sectors[index].Flags += Content::Assets::SectorFlag::CogLinked;
			}
		}
		break;

		case Cog::Symbols::SymbolType::Surface: {
			int index = static_cast<int>(*jt);
			if(index >= 0) {
				Surfaces[index].Flags += Content::Assets::SurfaceFlag::CogLinked;
			}
		}
		break;

		case Cog::Symbols::SymbolType::Thing: {
			int index = static_cast<int>(*jt);
			if(index >= 0) {
				Things[index].Flags += Content::Assets::ThingFlag::CogLinked;
			}
		}
		break;

		case Cog::Symbols::SymbolType::Ai:
		case Cog::Symbols::SymbolType::Keyframe:
			// TODO: Handle AI and keyframe loading.
		default:
			break;
		}
	}
}

Gorc::Game::World::Level::LevelModel::LevelModel(Gorc::Content::Manager& ContentManager, Cog::Compiler& CogCompiler, const Gorc::Content::Assets::Level& Level)
	: Level(Level), Adjoins(Level.Adjoins), Surfaces(Level.Surfaces), Sectors(Level.Sectors), MaterialCelNumber(Level.Materials.size(), 0),
	  Animations(OnAnimationDestroy),  Dispatcher(&CollisionConfiguration), DynamicsWorld(&Dispatcher, &Broadphase, &ConstraintSolver, &CollisionConfiguration),
	  SurfaceMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0))), SurfaceObjectData(Level.Surfaces.size()) {
	DynamicsWorld.setGravity(btVector3(0, 0, -Level.Header.WorldGravity));

	// Construct surface rigid bodies.
	for(size_t i = 0; i < Level.Surfaces.size(); ++i) {
		const auto& surf = Level.Surfaces[i];

		SurfaceRigidBodies.emplace_back(new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(
				0, &SurfaceMotionState, const_cast<btConvexHullShape*>(Level.SurfaceShapes[i].get()), btVector3(0,0,0))));

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

	CameraShape = std::unique_ptr<btSphereShape>(new btSphereShape(CameraRadius));

	// HACK: Create thing collision shapes and rigid bodies, enumerate spawn points
	for(const auto& thing : Level.Things) {
		CreateThing(thing, thing.Sector, thing.Position, thing.Orientation);
	}

	// HACK: Spawn camera thing
	CurrentSpawnPoint = 0;
	auto camera_thing_tuple = Things.Create();
	auto& camera_thing = *std::get<0>(camera_thing_tuple);
	CameraThingId = std::get<1>(camera_thing_tuple);

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
	camera_thing.ObjectData.SectorId = camera_thing.Sector;
	camera_thing.RigidBody->setUserPointer(&camera_thing.ObjectData);

	FlagSet<PhysicsCollideClass> CameraCollideType {PhysicsCollideClass::Player, PhysicsCollideClass::Thing};
	FlagSet<PhysicsCollideClass> CameraCollideWith {PhysicsCollideClass::Wall, PhysicsCollideClass::Adjoin, PhysicsCollideClass::Thing};

	DynamicsWorld.addRigidBody(camera_thing.RigidBody.get(), static_cast<unsigned int>(CameraCollideType),
			static_cast<unsigned int>(CameraCollideWith));

	//Broadphase.optimize();

	// Create COG script instances.
	for(const auto& cog : Level.Cogs) {
		Content::Assets::Script const* script = std::get<0>(cog);
		const std::vector<Cog::VM::Value>& values = std::get<1>(cog);

		AddCogInstance(script->Script, ContentManager, CogCompiler, values);
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
		new_thing.Position = pos;
		new_thing.Orientation = orient;
		new_thing.Type = Content::Assets::ThingType::Ghost;
		return std::get<1>(new_thing_tuple);
	}
	else if(tpl.Type == Content::Assets::ThingType::Actor) {
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
		if(new_thing.Move == Content::Assets::MoveType::Physics && new_thing.Collide != Content::Assets::CollideType::None) {
			thing_mass = new_thing.Mass;
		}

		btCollisionShape* thingShape = CameraShape.get();

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

		// Associate thing info structure.
		new_thing.ObjectData.ThingId = std::get<1>(new_thing_tuple);
		new_thing.ObjectData.SectorId = sector_num;
		new_thing.RigidBody->setUserPointer(&new_thing.ObjectData);

		if(new_thing.Move == Content::Assets::MoveType::Path && new_thing.Frames.size() > 0) {
			new_thing.RigidBody->setCollisionFlags(new_thing.RigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			new_thing.RigidBody->setActivationState(ISLAND_SLEEPING);
		}
		else {
			new_thing.RigidBody->setActivationState(ISLAND_SLEEPING);
		}

		new_thing.RigidBody->setAngularFactor(btVector3(0,0,0));

		DynamicsWorld.addRigidBody(new_thing.RigidBody.get(), static_cast<unsigned int>(CollideType),
				static_cast<unsigned int>(CollideWith));

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

		// Associate thing info structure.
		new_thing.ObjectData.ThingId = std::get<1>(new_thing_tuple);
		new_thing.ObjectData.SectorId = sector_num;
		new_thing.RigidBody->setUserPointer(&new_thing.ObjectData);

		if(new_thing.Move == Content::Assets::MoveType::Path && new_thing.Frames.size() > 0) {
			new_thing.RigidBody->setCollisionFlags(new_thing.RigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			new_thing.RigidBody->setActivationState(ISLAND_SLEEPING);
		}
		else {
			new_thing.RigidBody->setActivationState(ISLAND_SLEEPING);
		}

		DynamicsWorld.addRigidBody(new_thing.RigidBody.get(), static_cast<unsigned int>(CollideType),
				static_cast<unsigned int>(CollideWith));
		return std::get<1>(new_thing_tuple);
	}
	else {
		// TODO: Add ghostdecor thing.
		// For now, insert empty thing to fill ID.
		auto new_thing_tuple = Things.Create();
		auto& new_thing = *std::get<0>(new_thing_tuple);
		new_thing.Type = Content::Assets::ThingType::Ghost;
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
