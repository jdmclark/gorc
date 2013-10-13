#pragma once

#include "framework/math/vector.h"
#include "framework/math/box.h"
#include "sound.h"
#include "content/flags/sector_flag.h"

namespace Gorc {
namespace Content {
namespace Assets {

class LevelSector {
public:
	size_t Number;
	FlagSet<Flags::SectorFlag> Flags;
	float AmbientLight;
	float ExtraLight;
	size_t ColorMap;
	Math::Vector<3> Tint;
	Math::Box<3> BoundingBox;
	Math::Box<3> CollideBox;
	Sound const* AmbientSound = nullptr;
	float AmbientSoundVolume = 0.0f;
	Math::Vector<3> Center;
	float Radius;
	std::vector<size_t> Vertices;
	size_t FirstSurface;
	size_t SurfaceCount;
	Math::Vector<3> Thrust;
};

}
}
}
