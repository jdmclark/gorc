#include "cogcontroller.h"
#include "game/world/level/levelpresenter.h"
#include "game/world/level/thing.h"
#include "game/world/level/levelmodel.h"
#include "game/constants.h"
#include <iostream>

using namespace Gorc::Math;

void Gorc::Game::World::Level::Gameplay::CogController::Update(int thing_id, double dt) {
	Thing& thing = presenter.Model->Things[thing_id];
	if(thing.PathMoving) {
		UpdateThingPathMoving(thing_id, thing, dt);
	}

	// Update thing attachment motion
	if(thing.AttachFlags & Flags::AttachFlag::AttachedToThing) {
		Thing& attached_thing = presenter.Model->Things[thing.AttachedThing];
		Vector<3> deltaPosition = attached_thing.Position - thing.PrevAttachedThingPosition;

		if(Math::Length2(deltaPosition) > 0.0f) {
			thing.RigidBody->setActivationState(DISABLE_DEACTIVATION);
			btTransform currentWorldTransform;
			thing.RigidBody->getMotionState()->getWorldTransform(currentWorldTransform);
			Vector<3> targetPosition = VecBt(currentWorldTransform.getOrigin()) + deltaPosition;
			thing.RigidBody->getMotionState()->setWorldTransform(btTransform(currentWorldTransform.getRotation(), BtVec(targetPosition)));
			thing.PrevAttachedThingPosition = attached_thing.Position;
		}
		else {
			thing.RigidBody->setActivationState(0);
		}
	}
}

void Gorc::Game::World::Level::Gameplay::CogController::UpdateThingPathMoving(int thing_id, Thing& thing, double dt) {
	auto target_position_tuple = thing.Frames[thing.NextFrame];
	Vector<3> targetPosition = std::get<0>(target_position_tuple);
	Vector<3> orient = std::get<1>(target_position_tuple);

	btQuaternion targetOrientQuat(0.0f, 0.0f, 0.0f, 1.0f);
	targetOrientQuat *= btQuaternion(btVector3(0,0,1), Deg2Rad * Math::Get<1>(orient)); // Yaw
	targetOrientQuat *= btQuaternion(btVector3(1,0,0), Deg2Rad * Math::Get<0>(orient)); // Pitch
	targetOrientQuat *= btQuaternion(btVector3(0,1,0), Deg2Rad * Math::Get<2>(orient)); // Roll

	btTransform currentWorldTransform;
	thing.RigidBody->getMotionState()->getWorldTransform(currentWorldTransform);

	Vector<3> currentPosition = VecBt(currentWorldTransform.getOrigin());

	// PathMoveSpeed seems to be some factor of distance per frame, and Jedi has a different framerate.
	// Use a magic multiple to correct it.
	float dist_len = Math::Length(targetPosition - currentPosition);
	float alpha = RateFactor * dt * thing.PathMoveSpeed / dist_len;
	if(alpha >= 1.0f || dist_len == 0.0f) {
		btTransform targetWorldTransform(targetOrientQuat, BtVec(targetPosition));
		thing.RigidBody->getMotionState()->setWorldTransform(targetWorldTransform);

		// Arrived at next frame. Advance to next.
		thing.CurrentFrame = thing.NextFrame;
		if(thing.CurrentFrame == thing.GoalFrame) {
			thing.PathMoving = false;
			thing.PathMoveSpeed = 0.0f;
			thing.RigidBody->setActivationState(0);
			presenter.SoundPresenter.StopFoleyLoop(thing_id);
			presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::StopMove);

			// Dispatch cog messages and resume cogs which are waiting for stop.
			presenter.ScriptPresenter.SendMessageToLinked(Cog::MessageId::Arrived, static_cast<int>(thing_id), Flags::MessageType::Thing);
			presenter.ScriptPresenter.ResumeWaitForStop(thing_id);
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

void Gorc::Game::World::Level::Gameplay::CogController::CreateControllerData(int thing_id) {
	auto& new_thing = presenter.Model->Things[thing_id];

	btQuaternion orientation(0.0f, 0.0f, 0.0f, 1.0f);
	orientation *= btQuaternion(btVector3(0,0,1), Deg2Rad * Math::Get<1>(new_thing.Orientation)); // Yaw
	orientation *= btQuaternion(btVector3(1,0,0), Deg2Rad * Math::Get<0>(new_thing.Orientation)); // Pitch
	orientation *= btQuaternion(btVector3(0,1,0), Deg2Rad * Math::Get<2>(new_thing.Orientation)); // Roll

	float thing_mass = 0.0f;

	FlagSet<PhysicsCollideClass> CollideType { PhysicsCollideClass::Thing };
	FlagSet<PhysicsCollideClass> CollideWith;

	if(new_thing.Collide != Flags::CollideType::None) {
		CollideWith = { PhysicsCollideClass::Thing };
	}

	btCollisionShape* thingShape;
	if(new_thing.Flags & FlagSet<Flags::ThingFlag>{ Flags::ThingFlag::PartOfWorldGeometry, Flags::ThingFlag::CanStandOn }) {
		thingShape = new_thing.Model3d->Shape.get();
		CollideType += PhysicsCollideClass::Floor;
	}
	else {
		new_thing.ActorCollideShape = std::unique_ptr<btCollisionShape>(new btSphereShape(new_thing.Size * 0.5f));
		thingShape = new_thing.ActorCollideShape.get();
	}

	btVector3 thing_inertia(0,0,0);
	thingShape->calculateLocalInertia(thing_mass, thing_inertia);

	new_thing.MotionState = std::unique_ptr<btDefaultMotionState>(new btDefaultMotionState(
			btTransform(orientation, Math::BtVec(new_thing.Position))));
	new_thing.RigidBody = std::unique_ptr<btRigidBody>(new btRigidBody(
			btRigidBody::btRigidBodyConstructionInfo(thing_mass, new_thing.MotionState.get(),
					thingShape, thing_inertia)));

	// Associate thing info structure.
	new_thing.ObjectData.CollisionGroup = CollideType;
	new_thing.ObjectData.CollisionMask = CollideWith;

	new_thing.RigidBody->setUserPointer(&new_thing.ObjectData);

	new_thing.RigidBody->setCollisionFlags(new_thing.RigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	new_thing.RigidBody->setActivationState(ISLAND_SLEEPING);

	presenter.Model->DynamicsWorld.addRigidBody(new_thing.RigidBody.get());
}
