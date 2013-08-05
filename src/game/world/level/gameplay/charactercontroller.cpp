#include "charactercontroller.h"
#include "game/world/level/levelpresenter.h"
#include "game/world/level/levelmodel.h"

void Gorc::Game::World::Level::Gameplay::CharacterController::Update(unsigned int thing_id, double dt) {
	// TODO
}

unsigned int Gorc::Game::World::Level::Gameplay::CharacterController::Create(const Content::Assets::Template& tpl,
		unsigned int sector_num, const Math::Vector<3>& pos, const Math::Vector<3>& orient) {
	auto new_thing_tuple = presenter.Model->Things.Create();
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
		BtVec(Vec(0.0f, 0.0f, 0.02f) - new_thing.Model3d->InsertOffset)
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

	presenter.Model->DynamicsWorld.addRigidBody(new_thing.RigidBody.get());

	return std::get<1>(new_thing_tuple);
}
