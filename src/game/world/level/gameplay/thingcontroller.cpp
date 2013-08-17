#include "thingcontroller.h"
#include "game/world/level/levelpresenter.h"
#include "game/world/level/levelmodel.h"
#include "game/constants.h"

Gorc::Game::World::Level::Gameplay::ThingController::ThingController(LevelPresenter& presenter)
	: presenter(presenter) {
	return;
}

Gorc::Game::World::Level::Gameplay::ThingController::~ThingController() {
	return;
}

void Gorc::Game::World::Level::Gameplay::ThingController::HandleAnimationMarker(int thing_id, Flags::KeyMarkerType marker) {
	return;
}

void Gorc::Game::World::Level::Gameplay::ThingController::CreateControllerData(int thing_id) {
	auto& new_thing = presenter.Model->Things[thing_id];

	btQuaternion orientation(btVector3(0,0,1), Deg2Rad * Math::Get<1>(new_thing.Orientation));

	float thing_mass = 0.0f;
	if(new_thing.PhysicsFlags & Flags::PhysicsFlag::HasGravity) {
		thing_mass = new_thing.Mass;
	}

	new_thing.ActorCollideShape = std::unique_ptr<btCollisionShape>(new btSphereShape(new_thing.Size * PhysicsWorldScale));
	btCollisionShape* thingShape = new_thing.ActorCollideShape.get();

	btVector3 thing_inertia(0,0,0);
	thingShape->calculateLocalInertia(thing_mass, thing_inertia);

	new_thing.MotionState = std::unique_ptr<btDefaultMotionState>(new btDefaultMotionState(
			btTransform(orientation, Math::BtVec(new_thing.Position) * PhysicsWorldScale)));

	btRigidBody::btRigidBodyConstructionInfo actor_ci(thing_mass, new_thing.MotionState.get(),
			thingShape, thing_inertia);
	new_thing.RigidBody = std::unique_ptr<btRigidBody>(new btRigidBody(actor_ci));

	// Associate thing info structure.
	new_thing.ObjectData.CollisionGroup = FlagSet<PhysicsCollideClass>();
	new_thing.ObjectData.CollisionMask = FlagSet<PhysicsCollideClass>();
	new_thing.RigidBody->setUserPointer(&new_thing.ObjectData);

	presenter.Model->DynamicsWorld.addRigidBody(new_thing.RigidBody.get());
}

void Gorc::Game::World::Level::Gameplay::ThingController::RemoveControllerData(int thing_id) {
	auto& thing = presenter.Model->Things[thing_id];
	presenter.Model->DynamicsWorld.removeRigidBody(thing.RigidBody.get());
}

void Gorc::Game::World::Level::Gameplay::ThingController::Taken(int thing_id, int player_id) {
	// Only items can be taken.
	return;
}
