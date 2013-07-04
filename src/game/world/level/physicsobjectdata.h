#pragma once

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class PhysicsObjectData {
public:
	virtual ~PhysicsObjectData();
};

class SurfaceObjectData : public PhysicsObjectData {
public:
	unsigned int SectorId;
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
