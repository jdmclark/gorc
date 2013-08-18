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

void Gorc::Game::World::Level::Gameplay::ThingController::Update(int thing_id, double dt) {
	auto& thing = presenter.Model->Things[thing_id];
	if(thing.Timer > 0.0f) {
		thing.Timer -= dt;
		if(thing.Timer <= 0.0f) {
			presenter.DestroyThing(thing_id);
		}
	}
}

void Gorc::Game::World::Level::Gameplay::ThingController::HandleAnimationMarker(int thing_id, Flags::KeyMarkerType marker) {
	return;
}

void Gorc::Game::World::Level::Gameplay::ThingController::CreateControllerData(int thing_id) {
	auto& new_thing = presenter.Model->Things[thing_id];

	btQuaternion orientation(0.0f, 0.0f, 0.0f, 1.0f);
	orientation *= btQuaternion(btVector3(0,1,0), Deg2Rad * Math::Get<2>(new_thing.Orientation)); // Roll
	orientation *= btQuaternion(btVector3(0,0,1), Deg2Rad * Math::Get<1>(new_thing.Orientation)); // Yaw
	orientation *= btQuaternion(btVector3(1,0,0), Deg2Rad * Math::Get<0>(new_thing.Orientation)); // Pitch

	float thing_mass = new_thing.Mass;
	if(new_thing.Move != Flags::MoveType::Physics) {
		thing_mass = 0.0f;
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
	new_thing.RigidBody->setCcdMotionThreshold(1e-10f);
	new_thing.RigidBody->setCcdSweptSphereRadius(new_thing.Size * 0.25f * PhysicsWorldScale);

	presenter.Model->DynamicsWorld.addRigidBody(new_thing.RigidBody.get());

	if(!(new_thing.PhysicsFlags & Flags::PhysicsFlag::HasGravity)) {
		new_thing.RigidBody->setGravity(btVector3(0,0,0));
	}

	new_thing.RigidBody->setActivationState(ISLAND_SLEEPING);
}

void Gorc::Game::World::Level::Gameplay::ThingController::RemoveControllerData(int thing_id) {
	auto& thing = presenter.Model->Things[thing_id];
	presenter.Model->DynamicsWorld.removeRigidBody(thing.RigidBody.get());
}

void Gorc::Game::World::Level::Gameplay::ThingController::Taken(int thing_id, int player_id) {
	return;
}

void Gorc::Game::World::Level::Gameplay::ThingController::TouchedThing(int thing_id, int touched_thing_id) {
	// Dispatch touched cog message.
	presenter.ScriptPresenter.SendMessageToLinked(Cog::MessageId::Touched,
			thing_id, Flags::MessageType::Thing,
			touched_thing_id, Flags::MessageType::Thing);
	return;
}

void Gorc::Game::World::Level::Gameplay::ThingController::TouchedSurface(int thing_id, int touched_surface_id) {
	return;
}
