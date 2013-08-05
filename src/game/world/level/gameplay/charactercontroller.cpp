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

void Gorc::Game::World::Level::Gameplay::CharacterController::RunFallingSweep(unsigned int thing_id, Thing& thing,
		double dt, FilteredClosestRayResultCallback& rrcb) {
	// Test for collision between legs and ground
	Math::Vector<3> leg_height = thing.Model3d->InsertOffset;
	Math::Vector<3> leg_bottom = thing.Position - leg_height;

	rrcb = FilteredClosestRayResultCallback(Math::BtVec(thing.Position), Math::BtVec(leg_bottom));
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
}

void Gorc::Game::World::Level::Gameplay::CharacterController::RunWalkingSweep(unsigned int thing_id, Thing& thing,
		double dt, FilteredClosestRayResultCallback& rrcb) {
	// Test for collision between legs and ground
	Math::Vector<3> leg_height = thing.Model3d->InsertOffset * 1.50f;
	Math::Vector<3> leg_bottom = thing.Position - leg_height;

	rrcb = FilteredClosestRayResultCallback(Math::BtVec(thing.Position), Math::BtVec(leg_bottom));
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
}

void Gorc::Game::World::Level::Gameplay::CharacterController::UpdateFalling(unsigned int thing_id, Thing& thing, double dt) {
	FilteredClosestRayResultCallback rrcb(btVector3(0,0,0), btVector3(0,0,0));
	RunFallingSweep(thing_id, thing, dt, rrcb);

	thing.RigidBody->applyCentralImpulse(btVector3(Math::Get<0>(thing.Thrust), Math::Get<1>(thing.Thrust), 0.0f));

	if(rrcb.hasHit() && thing.RigidBody->getLinearVelocity().getZ() < 0.0f) {
		// Check if attached surface/thing has changed.
		PhysicsObjectData* userData = reinterpret_cast<PhysicsObjectData*>(rrcb.m_collisionObject->getUserPointer());
		SurfaceObjectData* surfaceObjectData = dynamic_cast<SurfaceObjectData*>(userData);
		ThingObjectData* thingObjectData = dynamic_cast<ThingObjectData*>(userData);

		if(surfaceObjectData) {
			LandOnSurface(thing_id, thing, surfaceObjectData->SurfaceId, rrcb);
		}
		else if(thingObjectData) {
			LandOnThing(thing_id, thing, thingObjectData->ThingId, rrcb);
		}
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::UpdateStandingOnSurface(unsigned int thing_id, Thing& thing, double dt) {
	FilteredClosestRayResultCallback rrcb(btVector3(0,0,0), btVector3(0,0,0));
	RunWalkingSweep(thing_id, thing, dt, rrcb);

	if(!rrcb.hasHit()) {
		// Player is falling again.
		thing.RigidBody->setGravity(btVector3(0,0,-presenter.Model->Header.WorldGravity));
		thing.AttachFlags = FlagSet<Flags::AttachFlag>();
	}
	else {
		// Check if attached surface/thing has changed.
		PhysicsObjectData* userData = reinterpret_cast<PhysicsObjectData*>(rrcb.m_collisionObject->getUserPointer());
		SurfaceObjectData* surfaceUserData = dynamic_cast<SurfaceObjectData*>(userData);
		ThingObjectData* thingUserData = dynamic_cast<ThingObjectData*>(userData);
		if(surfaceUserData && surfaceUserData->SurfaceId != thing.AttachedSurface) {
			// Player has landed on a new surface.
			StepOnSurface(thing_id, thing, surfaceUserData->SurfaceId, rrcb);
		}
		else if(thingUserData) {
			StepOnThing(thing_id, thing, thingUserData->ThingId, rrcb);
		}

		thing.RigidBody->setGravity(btVector3(0,0,0));

		if(Math::Get<2>(thing.Thrust) > 0.0f) {
			Jump(thing_id, thing);
		}
		else {
			// Accelerate body along surface
			Math::Vector<3> hit_normal = Math::VecBt(rrcb.m_hitNormalWorld);
			Math::Vector<3> new_vel = thing.Thrust - hit_normal * Math::Dot(thing.Thrust, hit_normal);
			thing.RigidBody->setLinearVelocity(BtVec(new_vel));
		}
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::UpdateStandingOnThing(unsigned int thing_id, Thing& thing, double dt) {
	FilteredClosestRayResultCallback rrcb(btVector3(0,0,0), btVector3(0,0,0));
	RunWalkingSweep(thing_id, thing, dt, rrcb);

	if(!rrcb.hasHit()) {
		// Player is falling again.
		thing.RigidBody->setGravity(btVector3(0,0,-presenter.Model->Header.WorldGravity));
		thing.AttachFlags = FlagSet<Flags::AttachFlag>();
	}
	else {
		// Check if attached surface/thing has changed.
		PhysicsObjectData* userData = reinterpret_cast<PhysicsObjectData*>(rrcb.m_collisionObject->getUserPointer());
		SurfaceObjectData* surfaceUserData = dynamic_cast<SurfaceObjectData*>(userData);
		ThingObjectData* thingUserData = dynamic_cast<ThingObjectData*>(userData);
		if(thingUserData && thingUserData->ThingId != thing.AttachedThing) {
			// Player has landed on a new thing.
			StepOnThing(thing_id, thing, thingUserData->ThingId, rrcb);
		}
		else if(surfaceUserData) {
			StepOnSurface(thing_id, thing, surfaceUserData->SurfaceId, rrcb);
		}

		thing.RigidBody->setGravity(btVector3(0,0,0));

		if(Math::Get<2>(thing.Thrust) > 0.0f) {
			Jump(thing_id, thing);
		}
		else {
			// Accelerate body along surface
			Math::Vector<3> hit_normal = Math::VecBt(rrcb.m_hitNormalWorld);
			Math::Vector<3> new_vel = thing.Thrust - hit_normal * Math::Dot(thing.Thrust, hit_normal);

			if(thingUserData) {
				new_vel += (presenter.Model->Things[thingUserData->ThingId].Position - thing.PrevAttachedThingPosition) / dt;
				thing.PrevAttachedThingPosition = presenter.Model->Things[thingUserData->ThingId].Position;
			}

			thing.RigidBody->setLinearVelocity(BtVec(new_vel));
		}
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::StepOnSurface(unsigned int thing_id, Thing& thing, unsigned int surf_id,
		const FilteredClosestRayResultCallback& rrcb) {
	thing.AttachFlags = FlagSet<Flags::AttachFlag> { Flags::AttachFlag::AttachedToWorldSurface };
	thing.AttachedSurface = surf_id;

	// Player has landed.
	presenter.AdjustThingPosition(thing_id, Math::VecBt(rrcb.m_hitPointWorld) + thing.Model3d->InsertOffset);
	thing.RigidBody->setGravity(btVector3(0,0,0));
}

void Gorc::Game::World::Level::Gameplay::CharacterController::StepOnThing(unsigned int thing_id, Thing& thing, unsigned int land_thing_id,
		const FilteredClosestRayResultCallback& rrcb) {
	thing.AttachFlags = FlagSet<Flags::AttachFlag> { Flags::AttachFlag::AttachedToThingFace };
	thing.AttachedThing = land_thing_id;
	thing.PrevAttachedThingPosition = presenter.Model->Things[land_thing_id].Position;

	// Player has landed.
	presenter.AdjustThingPosition(thing_id, Math::VecBt(rrcb.m_hitPointWorld) + thing.Model3d->InsertOffset);
	thing.RigidBody->setGravity(btVector3(0,0,0));
}

void Gorc::Game::World::Level::Gameplay::CharacterController::LandOnSurface(unsigned int thing_id, Thing& thing, unsigned int surf_id,
		const FilteredClosestRayResultCallback& rrcb) {
	StepOnSurface(thing_id, thing, surf_id, rrcb);

	const Content::Assets::LevelSurface& surf = presenter.Model->Surfaces[surf_id];

	Flags::SoundSubclassType subclass = Flags::SoundSubclassType::LandHard;
	if(surf.Flags & Flags::SurfaceFlag::Metal) {
		subclass = Flags::SoundSubclassType::LandMetal;
	}
	else if(surf.Flags & Flags::SurfaceFlag::Dirt) {
		subclass = Flags::SoundSubclassType::LandEarth;
	}
	else if(surf.Flags & Flags::SurfaceFlag::ShallowWater) {
		subclass = Flags::SoundSubclassType::LandPuddle;
	}
	else if(surf.Flags & Flags::SurfaceFlag::DeepWater) {
		subclass = Flags::SoundSubclassType::LandWater;
	}
	else if(surf.Flags & Flags::SurfaceFlag::VeryDeepWater) {
		subclass = Flags::SoundSubclassType::LandWater;
	}

	presenter.SoundPresenter.PlaySoundClass(thing_id, subclass);
}

void Gorc::Game::World::Level::Gameplay::CharacterController::LandOnThing(unsigned int thing_id, Thing& thing, unsigned int land_thing_id,
		const FilteredClosestRayResultCallback& rrcb) {
	StepOnThing(thing_id, thing, land_thing_id, rrcb);

	FlagSet<Flags::ThingFlag> flags = presenter.Model->Things[land_thing_id].Flags;

	Flags::SoundSubclassType subclass = Flags::SoundSubclassType::LandHard;
	if(flags & Flags::ThingFlag::Metal) {
		subclass = Flags::SoundSubclassType::LandMetal;
	}
	else if(flags & Flags::ThingFlag::Dirt) {
		subclass = Flags::SoundSubclassType::LandEarth;
	}

	presenter.SoundPresenter.PlaySoundClass(thing_id, subclass);
}

void Gorc::Game::World::Level::Gameplay::CharacterController::Jump(unsigned int thing_id, Thing& thing) {
	if(thing.AttachFlags & Flags::AttachFlag::AttachedToWorldSurface) {
		JumpFromSurface(thing_id, thing, thing.AttachedSurface);
	}
	else if(thing.AttachFlags & Flags::AttachFlag::AttachedToThingFace) {
		JumpFromThing(thing_id, thing, thing.AttachedThing);
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::JumpFromSurface(unsigned int thing_id, Thing& thing, unsigned int surf_id) {
	thing.RigidBody->setGravity(btVector3(0,0,-presenter.Model->Header.WorldGravity));
	thing.AttachFlags = FlagSet<Flags::AttachFlag>();
	thing.RigidBody->setLinearVelocity(BtVec(thing.Thrust));

	const Content::Assets::LevelSurface& surf = presenter.Model->Surfaces[surf_id];

	Flags::SoundSubclassType subclass = Flags::SoundSubclassType::Jump;
	if(surf.Flags & Flags::SurfaceFlag::Metal) {
		subclass = Flags::SoundSubclassType::JumpMetal;
	}
	else if(surf.Flags & Flags::SurfaceFlag::Dirt) {
		subclass = Flags::SoundSubclassType::JumpEarth;
	}
	else if(surf.Flags & Flags::SurfaceFlag::ShallowWater) {
		subclass = Flags::SoundSubclassType::JumpWater;
	}
	else if(surf.Flags & Flags::SurfaceFlag::DeepWater) {
		subclass = Flags::SoundSubclassType::JumpWater;
	}
	else if(surf.Flags & Flags::SurfaceFlag::VeryDeepWater) {
		subclass = Flags::SoundSubclassType::JumpWater;
	}

	presenter.SoundPresenter.PlaySoundClass(thing_id, subclass);
}

void Gorc::Game::World::Level::Gameplay::CharacterController::JumpFromThing(unsigned int thing_id, Thing& thing, unsigned int jump_thing_id) {
	thing.RigidBody->setGravity(btVector3(0,0,-presenter.Model->Header.WorldGravity));
	thing.AttachFlags = FlagSet<Flags::AttachFlag>();
	thing.RigidBody->setLinearVelocity(BtVec(thing.Thrust));

	FlagSet<Flags::ThingFlag> flags = presenter.Model->Things[jump_thing_id].Flags;

	Flags::SoundSubclassType subclass = Flags::SoundSubclassType::Jump;
	if(flags & Flags::ThingFlag::Metal) {
		subclass = Flags::SoundSubclassType::JumpMetal;
	}
	else if(flags & Flags::ThingFlag::Dirt) {
		subclass = Flags::SoundSubclassType::JumpEarth;
	}

	presenter.SoundPresenter.PlaySoundClass(thing_id, subclass);
}

void Gorc::Game::World::Level::Gameplay::CharacterController::Update(unsigned int thing_id, double dt) {
	Thing& thing = presenter.Model->Things[thing_id];

	// Update actor state
	if(thing.AttachFlags & Flags::AttachFlag::AttachedToWorldSurface) {
		UpdateStandingOnSurface(thing_id, thing, dt);
	}
	else if(thing.AttachFlags & Flags::AttachFlag::AttachedToThingFace) {
		UpdateStandingOnThing(thing_id, thing, dt);
	}
	else {
		UpdateFalling(thing_id, thing, dt);
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
	new_thing.RigidBody->setAngularFactor(btVector3(0,0,0));
	new_thing.RigidBody->setSleepingThresholds(0.0f, 0.0f);
	new_thing.RigidBody->setActivationState(DISABLE_DEACTIVATION);

	presenter.Model->DynamicsWorld.addRigidBody(new_thing.RigidBody.get());

	return std::get<1>(new_thing_tuple);
}
