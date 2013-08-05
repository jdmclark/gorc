#include "charactercontroller.h"
#include "game/world/level/levelpresenter.h"
#include "game/world/level/levelmodel.h"
#include "game/world/level/physics/physicsobjectdata.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Gameplay {

class FilteredClosestRayResultCallback : public btCollisionWorld::ClosestRayResultCallback {
public:
	using ClosestRayResultCallback::ClosestRayResultCallback;

	virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace) {
		btCollisionObject* obj = rayResult.m_collisionObject;
		PhysicsObjectData* userData = reinterpret_cast<PhysicsObjectData*>(obj->getUserPointer());
		if(userData) {
			bool maskCollides = (FlagSet<PhysicsCollideClass>(m_collisionFilterGroup) & userData->CollisionMask) != 0;
			maskCollides = maskCollides && (userData->CollisionGroup & FlagSet<PhysicsCollideClass>(m_collisionFilterMask)) != 0;
			if(!maskCollides) {
				return 1.0f;
			}
		}

		return btCollisionWorld::ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
	}
};

}
}
}
}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::Update(unsigned int thing_id, double dt) {
	Thing& thing = presenter.Model->Things[thing_id];

	// Test for collision between legs and ground
	Math::Vector<3> leg_height = thing.Model3d->InsertOffset;
	Math::Vector<3> leg_bottom = thing.Position - leg_height;
	leg_height *= 0.999f;

	FilteredClosestRayResultCallback rrcb(Math::BtVec(thing.Position), Math::BtVec(leg_bottom));

	FlagSet<PhysicsCollideClass> CollideClass { PhysicsCollideClass::Thing };
	switch(thing.Type) {
	case Flags::ThingType::Player:
		CollideClass += PhysicsCollideClass::Player;
		break;

	default:
	case Flags::ThingType::Actor:
		CollideClass += PhysicsCollideClass::Enemy;
		break;
	}

	rrcb.m_collisionFilterGroup = static_cast<unsigned int>(CollideClass);
	rrcb.m_collisionFilterMask = static_cast<unsigned int>(PhysicsCollideClass::Floor);

	presenter.Model->DynamicsWorld.rayTest(Math::BtVec(thing.Position), Math::BtVec(leg_bottom), rrcb);

	if(thing.AttachFlags & FlagSet<Flags::AttachFlag> { Flags::AttachFlag::AttachedToWorldSurface, Flags::AttachFlag::AttachedToThingFace }) {
		// Handle walking

		if(!rrcb.hasHit()) {
			// Player is falling again.
			thing.RigidBody->setGravity(btVector3(0,0,-presenter.Model->Header.WorldGravity));
			thing.AttachFlags = FlagSet<Flags::AttachFlag>();
		}
		else {
			Math::Vector<3> hit_normal = Math::VecBt(rrcb.m_hitNormalWorld);
			Math::Vector<3> new_vel = thing.Thrust - hit_normal * Math::Dot(thing.Thrust, hit_normal);

			// Check if attached surface/thing has changed.
			PhysicsObjectData* userData = reinterpret_cast<PhysicsObjectData*>(rrcb.m_collisionObject->getUserPointer());

			if(thing.AttachFlags & Flags::AttachFlag::AttachedToWorldSurface) {
				SurfaceObjectData* surfaceUserData = dynamic_cast<SurfaceObjectData*>(userData);
				if(!surfaceUserData || surfaceUserData->SurfaceId != thing.AttachedSurface) {
					// Player is falling again.
					thing.RigidBody->setGravity(btVector3(0,0,-presenter.Model->Header.WorldGravity));
					thing.AttachFlags = FlagSet<Flags::AttachFlag>();
					return;
				}
			}
			else if(thing.AttachFlags & Flags::AttachFlag::AttachedToThingFace) {
				ThingObjectData* thingUserData = dynamic_cast<ThingObjectData*>(userData);
				if(!thingUserData || thingUserData->ThingId != thing.AttachedThing) {
					// Player is falling again.
					thing.RigidBody->setGravity(btVector3(0,0,-presenter.Model->Header.WorldGravity));
					thing.AttachFlags = FlagSet<Flags::AttachFlag>();
					return;
				}
				else if(thingUserData) {
					new_vel += (presenter.Model->Things[thingUserData->ThingId].Position - thing.PrevAttachedThingPosition) / dt;
					thing.PrevAttachedThingPosition = presenter.Model->Things[thingUserData->ThingId].Position;
				}
			}

			thing.RigidBody->setGravity(btVector3(0,0,0));

			if(Math::Get<2>(thing.Thrust) > 0.0f) {
				// Thing is jumping.
				presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::Jump);
				thing.RigidBody->setGravity(btVector3(0,0,-presenter.Model->Header.WorldGravity));
				thing.AttachFlags = FlagSet<Flags::AttachFlag>();
				thing.RigidBody->setLinearVelocity(BtVec(thing.Thrust));
			}
			else {
				// Accelerate body along surface
				thing.RigidBody->setLinearVelocity(BtVec(new_vel));
			}
		}
	}
	else {
		// Handle falling
		thing.RigidBody->applyCentralImpulse(btVector3(Math::Get<0>(thing.Thrust), Math::Get<1>(thing.Thrust), 0.0f));

		if(rrcb.hasHit() && thing.RigidBody->getLinearVelocity().getZ() < 0.0f) {
			// Player has landed.
			presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::LandHard);
			thing.Position = Math::VecBt(rrcb.m_hitPointWorld) + leg_height;
			thing.RigidBody->proceedToTransform(btTransform(btQuaternion(0,0,0,1), Math::BtVec(thing.Position)));
			thing.RigidBody->setGravity(btVector3(0,0,0));

			// Check if attached surface/thing has changed.
			PhysicsObjectData* userData = reinterpret_cast<PhysicsObjectData*>(rrcb.m_collisionObject->getUserPointer());
			SurfaceObjectData* surfaceObjectData = dynamic_cast<SurfaceObjectData*>(userData);
			ThingObjectData* thingObjectData = dynamic_cast<ThingObjectData*>(userData);

			if(surfaceObjectData) {
				thing.AttachFlags = FlagSet<Flags::AttachFlag> { Flags::AttachFlag::AttachedToWorldSurface };
				thing.AttachedSurface = surfaceObjectData->SurfaceId;
			}
			else if(thingObjectData) {
				thing.AttachFlags = FlagSet<Flags::AttachFlag> { Flags::AttachFlag::AttachedToThingFace };
				thing.AttachedThing = thingObjectData->ThingId;
				thing.PrevAttachedThingPosition = presenter.Model->Things[thingObjectData->ThingId].Position;
			}
		}
	}
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
	btQuaternion orientation(btVector3(0,0,1), deg2rad * Math::Get<1>(orient));

	float thing_mass = new_thing.Mass;
	new_thing.ActorCollideShape = std::unique_ptr<btCollisionShape>(new btSphereShape(0.04f));
	btCollisionShape* thingShape = new_thing.ActorCollideShape.get();

	btVector3 thing_inertia(0,0,0);
	thingShape->calculateLocalInertia(thing_mass, thing_inertia);

	new_thing.MotionState = std::unique_ptr<btDefaultMotionState>(new btDefaultMotionState(
			btTransform(orientation, Math::BtVec(pos))));

	btRigidBody::btRigidBodyConstructionInfo actor_ci(thing_mass, new_thing.MotionState.get(),
			thingShape, thing_inertia);
	new_thing.RigidBody = std::unique_ptr<btRigidBody>(new btRigidBody(actor_ci));

	FlagSet<PhysicsCollideClass> CollideType {PhysicsCollideClass::Thing};

	if(tpl.Type == Flags::ThingType::Player) {
		CollideType += PhysicsCollideClass::Player;
	}
	else {
		CollideType += PhysicsCollideClass::Enemy;
	}

	FlagSet<PhysicsCollideClass> CollideWith { PhysicsCollideClass::Wall, PhysicsCollideClass::Adjoin, PhysicsCollideClass::Thing };

	// Associate thing info structure.
	new_thing.ObjectData.ThingId = std::get<1>(new_thing_tuple);
	new_thing.ObjectData.SectorId = sector_num;
	new_thing.ObjectData.CollisionGroup = CollideType;
	new_thing.ObjectData.CollisionMask = CollideWith;
	new_thing.RigidBody->setUserPointer(&new_thing.ObjectData);
	new_thing.RigidBody->setActivationState(ISLAND_SLEEPING);
	new_thing.RigidBody->setAngularFactor(btVector3(0,0,0));

	presenter.Model->DynamicsWorld.addRigidBody(new_thing.RigidBody.get());

	return std::get<1>(new_thing_tuple);
}
