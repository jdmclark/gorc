#pragma once

#include "physicscollideclass.h"
#include "framework/flagset.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class Thing;

class PhysicsObjectData {
public:
	virtual ~PhysicsObjectData();

	unsigned int SectorId;
	FlagSet<PhysicsCollideClass> CollisionGroup, CollisionMask;
};

class SurfaceObjectData : public PhysicsObjectData {
public:
	unsigned int SurfaceId;
};

class ThingObjectData : public PhysicsObjectData {
public:
	int ThingId;
};

}
}
}
}
