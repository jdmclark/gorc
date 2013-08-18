#include "charactercontroller.h"
#include "game/world/level/levelpresenter.h"
#include "game/world/level/levelmodel.h"
#include "game/world/level/physics/physicsobjectdata.h"
#include "game/constants.h"

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

void Gorc::Game::World::Level::Gameplay::CharacterController::PlayRunningAnimation(int thing_id, Thing& thing, double speed) {
	if(thing.ActorWalkAnimation >= 0) {
		Keys::KeyState& keyState = presenter.Model->KeyModel.Keys[thing.ActorWalkAnimation];
		const Content::Assets::PuppetSubmode& submode = thing.Puppet->GetMode(Flags::PuppetModeType::Default).GetSubmode(Flags::PuppetSubmodeType::Run);
		if(keyState.Animation != submode.Animation) {
			keyState.AnimationTime = 0.0;
		}

		keyState.Animation = submode.Animation;
		keyState.HighPriority = submode.HiPriority;
		keyState.LowPriority = submode.LoPriority;
		keyState.Flags = FlagSet<Flags::KeyFlag>();
		keyState.Speed = speed;
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::PlayStandingAnimation(int thing_id, Thing& thing) {
	if(thing.ActorWalkAnimation >= 0) {
		Keys::KeyState& keyState = presenter.Model->KeyModel.Keys[thing.ActorWalkAnimation];
		const Content::Assets::PuppetSubmode& submode = thing.Puppet->GetMode(Flags::PuppetModeType::Default).GetSubmode(Flags::PuppetSubmodeType::Stand);
		keyState.Speed = 1.0;
		keyState.Animation = submode.Animation;
		keyState.HighPriority = submode.HiPriority;
		keyState.LowPriority = submode.LoPriority;
		keyState.Flags = FlagSet<Flags::KeyFlag>();
	}
}

Gorc::Game::World::Level::Gameplay::StandingMaterial Gorc::Game::World::Level::Gameplay::CharacterController::GetStandingMaterial(Thing& thing) {
	if(thing.AttachFlags & Flags::AttachFlag::AttachedToThingFace) {
		auto& floor_thing = presenter.Model->Things[thing.AttachedThing];
		if(floor_thing.Flags & Flags::ThingFlag::Metal) {
			return StandingMaterial::Metal;
		}
		else if(floor_thing.Flags & Flags::ThingFlag::Dirt) {
			return StandingMaterial::Dirt;
		}
		else {
			return StandingMaterial::Hard;
		}
	}
	else if(thing.AttachFlags & Flags::AttachFlag::AttachedToWorldSurface) {
		auto& floor_surf = presenter.Model->Surfaces[thing.AttachedSurface];

		if(floor_surf.Flags & Flags::SurfaceFlag::Metal) {
			return StandingMaterial::Metal;
		}
		else if(floor_surf.Flags & Flags::SurfaceFlag::Dirt) {
			return StandingMaterial::Dirt;
		}
		else if(floor_surf.Flags & Flags::SurfaceFlag::ShallowWater) {
			return StandingMaterial::ShallowWater;
		}
		else if(floor_surf.Flags & Flags::SurfaceFlag::DeepWater) {
			return StandingMaterial::DeepWater;
		}
		else if(floor_surf.Flags & Flags::SurfaceFlag::VeryDeepWater) {
			return StandingMaterial::VeryDeepWater;
		}
		else {
			return StandingMaterial::Hard;
		}
	}
	else {
		return StandingMaterial::None;
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::RunFallingSweep(int thing_id, Thing& thing,
		double dt, FilteredClosestRayResultCallback& rrcb) {
	// Test for collision between legs and ground
	Math::Vector<3> thing_position = thing.Position * PhysicsWorldScale;
	Math::Vector<3> leg_height = thing.Model3d->InsertOffset * PhysicsWorldScale;
	Math::Vector<3> leg_bottom = thing_position - leg_height;

	rrcb = FilteredClosestRayResultCallback(Math::BtVec(thing_position), Math::BtVec(leg_bottom));
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

	presenter.Model->DynamicsWorld.rayTest(Math::BtVec(thing_position), Math::BtVec(leg_bottom), rrcb);

	if(rrcb.hasHit()) {
		rrcb.m_hitPointWorld *= PhysicsInvWorldScale;
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::RunWalkingSweep(int thing_id, Thing& thing,
		double dt, FilteredClosestRayResultCallback& rrcb) {
	// Test for collision between legs and ground
	Math::Vector<3> thing_position = thing.Position * PhysicsWorldScale;
	Math::Vector<3> leg_height = thing.Model3d->InsertOffset * PhysicsWorldScale * 1.50f;
	Math::Vector<3> leg_bottom = thing_position - leg_height;

	rrcb = FilteredClosestRayResultCallback(Math::BtVec(thing_position), Math::BtVec(leg_bottom));
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

	presenter.Model->DynamicsWorld.rayTest(Math::BtVec(thing_position), Math::BtVec(leg_bottom), rrcb);

	if(rrcb.hasHit()) {
		rrcb.m_hitPointWorld *= PhysicsInvWorldScale;
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::UpdateFalling(int thing_id, Thing& thing, double dt) {
	FilteredClosestRayResultCallback rrcb(btVector3(0,0,0), btVector3(0,0,0));
	RunFallingSweep(thing_id, thing, dt, rrcb);

	thing.RigidBody->applyCentralImpulse(btVector3(Math::Get<0>(thing.Thrust), Math::Get<1>(thing.Thrust), 0.0f) * PhysicsWorldScale);
	PlayStandingAnimation(thing_id, thing);

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

void Gorc::Game::World::Level::Gameplay::CharacterController::UpdateStandingOnSurface(int thing_id, Thing& thing, double dt) {
	FilteredClosestRayResultCallback rrcb(btVector3(0,0,0), btVector3(0,0,0));
	RunWalkingSweep(thing_id, thing, dt, rrcb);

	if(!rrcb.hasHit()) {
		// Player is falling again.
		thing.RigidBody->setGravity(btVector3(0,0,-presenter.Model->Header.WorldGravity * PhysicsWorldScale));
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
			Math::Vector<3> hit_world = Math::VecBt(rrcb.m_hitPointWorld);
			Math::Vector<3> hit_normal = Math::VecBt(rrcb.m_hitNormalWorld);
			Math::Vector<3> player_new_vel = thing.Thrust - hit_normal * Math::Dot(thing.Thrust, hit_normal);
			Math::Vector<3> new_vel = player_new_vel;

			if(surfaceUserData) {
				new_vel += presenter.Model->Surfaces[surfaceUserData->SurfaceId].Thrust;
			}

			// Accelerate body toward standing position
			float model_height = Math::Get<2>(thing.Model3d->InsertOffset);
			float dist = Math::Dot(hit_normal, thing.Position - hit_world);
			new_vel += hit_normal * (model_height - dist) * 20.0f;

			thing.RigidBody->setLinearVelocity(BtVec(new_vel) * PhysicsWorldScale);

			// Update idle animation
			float player_new_vel_len = Math::Length(player_new_vel);
			if(player_new_vel_len > 0.0f) {
				PlayRunningAnimation(thing_id, thing, player_new_vel_len * 20.0f);
			}
			else {
				PlayStandingAnimation(thing_id, thing);
			}
		}
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::UpdateStandingOnThing(int thing_id, Thing& thing, double dt) {
	FilteredClosestRayResultCallback rrcb(btVector3(0,0,0), btVector3(0,0,0));
	RunWalkingSweep(thing_id, thing, dt, rrcb);

	if(!rrcb.hasHit()) {
		// Player is falling again.
		thing.RigidBody->setGravity(btVector3(0,0,-presenter.Model->Header.WorldGravity * PhysicsWorldScale));
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
			Math::Vector<3> hit_world = Math::VecBt(rrcb.m_hitPointWorld);
			Math::Vector<3> hit_normal = Math::VecBt(rrcb.m_hitNormalWorld);
			Math::Vector<3> player_new_vel = thing.Thrust - hit_normal * Math::Dot(thing.Thrust, hit_normal);
			Math::Vector<3> new_vel = player_new_vel;

			if(thingUserData) {
				new_vel += (presenter.Model->Things[thingUserData->ThingId].Position - thing.PrevAttachedThingPosition) / dt;
				thing.PrevAttachedThingPosition = presenter.Model->Things[thingUserData->ThingId].Position;
			}

			// Accelerate body toward standing position
			float model_height = Math::Get<2>(thing.Model3d->InsertOffset);
			float dist = Math::Dot(hit_normal, thing.Position - hit_world);
			new_vel += hit_normal * (model_height - dist) * 20.0f;

			thing.RigidBody->setLinearVelocity(BtVec(new_vel) * PhysicsWorldScale);

			// Update idle animation
			float player_new_vel_len = Math::Length(player_new_vel);
			if(player_new_vel_len > 0.0f) {
				PlayRunningAnimation(thing_id, thing, player_new_vel_len * 20.0f);
			}
			else {
				PlayStandingAnimation(thing_id, thing);
			}
		}
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::StepOnSurface(int thing_id, Thing& thing, unsigned int surf_id,
		const FilteredClosestRayResultCallback& rrcb) {
	thing.AttachFlags = FlagSet<Flags::AttachFlag> { Flags::AttachFlag::AttachedToWorldSurface };
	thing.AttachedSurface = surf_id;

	// Player has landed.
	//presenter.AdjustThingPosition(thing_id, Math::VecBt(rrcb.m_hitPointWorld) + thing.Model3d->InsertOffset);
	thing.RigidBody->setGravity(btVector3(0,0,0));
}

void Gorc::Game::World::Level::Gameplay::CharacterController::StepOnThing(int thing_id, Thing& thing, int land_thing_id,
		const FilteredClosestRayResultCallback& rrcb) {
	thing.AttachFlags = FlagSet<Flags::AttachFlag> { Flags::AttachFlag::AttachedToThingFace };
	thing.AttachedThing = land_thing_id;
	thing.PrevAttachedThingPosition = presenter.Model->Things[land_thing_id].Position;

	// Player has landed.
	//presenter.AdjustThingPosition(thing_id, Math::VecBt(rrcb.m_hitPointWorld) + thing.Model3d->InsertOffset);
	thing.RigidBody->setGravity(btVector3(0,0,0));
}

void Gorc::Game::World::Level::Gameplay::CharacterController::LandOnSurface(int thing_id, Thing& thing, unsigned int surf_id,
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

void Gorc::Game::World::Level::Gameplay::CharacterController::LandOnThing(int thing_id, Thing& thing, int land_thing_id,
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

void Gorc::Game::World::Level::Gameplay::CharacterController::Jump(int thing_id, Thing& thing) {
	if(thing.AttachFlags & Flags::AttachFlag::AttachedToWorldSurface) {
		JumpFromSurface(thing_id, thing, thing.AttachedSurface);
	}
	else if(thing.AttachFlags & Flags::AttachFlag::AttachedToThingFace) {
		JumpFromThing(thing_id, thing, thing.AttachedThing);
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::JumpFromSurface(int thing_id, Thing& thing, unsigned int surf_id) {
	thing.RigidBody->setGravity(btVector3(0,0,-presenter.Model->Header.WorldGravity * PhysicsWorldScale));
	thing.AttachFlags = FlagSet<Flags::AttachFlag>();
	thing.RigidBody->setLinearVelocity(BtVec(thing.Thrust) * PhysicsWorldScale);

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

void Gorc::Game::World::Level::Gameplay::CharacterController::JumpFromThing(int thing_id, Thing& thing, int jump_thing_id) {
	thing.RigidBody->setGravity(btVector3(0,0,-presenter.Model->Header.WorldGravity * PhysicsWorldScale));
	thing.AttachFlags = FlagSet<Flags::AttachFlag>();
	thing.RigidBody->setLinearVelocity(BtVec(thing.Thrust) * PhysicsWorldScale);

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

void Gorc::Game::World::Level::Gameplay::CharacterController::Update(int thing_id, double dt) {
	ThingController::Update(thing_id, dt);

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

void Gorc::Game::World::Level::Gameplay::CharacterController::CreateControllerData(int thing_id) {
	auto& new_thing = presenter.Model->Things[thing_id];

	btQuaternion orientation(btVector3(0,0,1), Deg2Rad * Math::Get<1>(new_thing.Orientation));

	float thing_mass = new_thing.Mass;
	new_thing.ActorCollideShape = std::unique_ptr<btCollisionShape>(new btSphereShape(0.04f * PhysicsWorldScale));
	btCollisionShape* thingShape = new_thing.ActorCollideShape.get();

	btVector3 thing_inertia(0,0,0);
	thingShape->calculateLocalInertia(thing_mass, thing_inertia);

	new_thing.MotionState = std::unique_ptr<btDefaultMotionState>(new btDefaultMotionState(
			btTransform(orientation, Math::BtVec(new_thing.Position) * PhysicsWorldScale)));

	btRigidBody::btRigidBodyConstructionInfo actor_ci(thing_mass, new_thing.MotionState.get(),
			thingShape, thing_inertia);
	new_thing.RigidBody = std::unique_ptr<btRigidBody>(new btRigidBody(actor_ci));

	FlagSet<PhysicsCollideClass> CollideType {PhysicsCollideClass::Thing};

	if(new_thing.Type == Flags::ThingType::Player) {
		CollideType += PhysicsCollideClass::Player;
	}
	else {
		CollideType += PhysicsCollideClass::Enemy;
	}

	FlagSet<PhysicsCollideClass> CollideWith { PhysicsCollideClass::Wall, PhysicsCollideClass::Adjoin, PhysicsCollideClass::Thing };

	// Associate thing info structure.
	new_thing.ObjectData.CollisionGroup = CollideType;
	new_thing.ObjectData.CollisionMask = CollideWith;
	new_thing.RigidBody->setUserPointer(&new_thing.ObjectData);
	new_thing.RigidBody->setAngularFactor(btVector3(0,0,0));
	new_thing.RigidBody->setSleepingThresholds(0.0f, 0.0f);
	new_thing.RigidBody->setActivationState(DISABLE_DEACTIVATION);
	new_thing.RigidBody->setCcdMotionThreshold(1e-10f);
	new_thing.RigidBody->setCcdSweptSphereRadius(0.01f * PhysicsWorldScale);

	presenter.Model->DynamicsWorld.addRigidBody(new_thing.RigidBody.get());

	// HACK: Initialize actor walk animation
	if(new_thing.Puppet) {
		new_thing.ActorWalkAnimation = presenter.KeyPresenter.PlayPuppetKey(thing_id, Flags::PuppetModeType::Default, Flags::PuppetSubmodeType::Stand);
		Keys::KeyState& keyState = presenter.Model->KeyModel.Keys[new_thing.ActorWalkAnimation];
		keyState.Flags = FlagSet<Flags::KeyFlag>();
	}
	else {
		new_thing.ActorWalkAnimation = -1;
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::HandleAnimationMarker(int thing_id, Flags::KeyMarkerType marker) {
	switch(marker) {
	case Flags::KeyMarkerType::LeftRunFootstep:
		PlayLeftRunFootstep(thing_id);
		break;

	case Flags::KeyMarkerType::RightRunFootstep:
		PlayRightRunFootstep(thing_id);
		break;
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::PlayLeftRunFootstep(int thing_id) {
	auto& thing = presenter.Model->Things[thing_id];
	StandingMaterial mat = GetStandingMaterial(thing);

	switch(mat) {
	case StandingMaterial::Metal:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::LRunMetal);
		break;

	case StandingMaterial::Dirt:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::LRunEarth);
		break;

	case StandingMaterial::ShallowWater:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::LRunPuddle);
		break;

	case StandingMaterial::DeepWater:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::LRunWater);
		break;

	case StandingMaterial::VeryDeepWater:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::LRunWater);
		break;

	default:
	case StandingMaterial::Hard:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::LRunHard);
		break;
	}
}

void Gorc::Game::World::Level::Gameplay::CharacterController::PlayRightRunFootstep(int thing_id) {
	auto& thing = presenter.Model->Things[thing_id];
	StandingMaterial mat = GetStandingMaterial(thing);

	switch(mat) {
	case StandingMaterial::Metal:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::RRunMetal);
		break;

	case StandingMaterial::Dirt:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::RRunEarth);
		break;

	case StandingMaterial::ShallowWater:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::RRunPuddle);
		break;

	case StandingMaterial::DeepWater:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::RRunWater);
		break;

	case StandingMaterial::VeryDeepWater:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::RRunWater);
		break;

	default:
	case StandingMaterial::Hard:
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::RRunHard);
		break;
	}
}
