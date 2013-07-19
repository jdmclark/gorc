#pragma once

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class PhysicsObjectData {
public:
	virtual ~PhysicsObjectData();

	unsigned int SectorId;
};

class SurfaceObjectData : public PhysicsObjectData {
public:
	unsigned int SurfaceId;
};

class ThingObjectData : public PhysicsObjectData {
public:
	unsigned int ThingId;
};

}
}
}
}
