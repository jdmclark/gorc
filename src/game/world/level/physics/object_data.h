#pragma once

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Physics {

class ObjectData {
public:
	virtual ~ObjectData();
};

class ThingObjectData : public ObjectData {
public:
	int ThingId;
};

class SurfaceObjectData : public ObjectData {
public:
	int SectorId;
	int SurfaceId;
};

}
}
}
}
}
