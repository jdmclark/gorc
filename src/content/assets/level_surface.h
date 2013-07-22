#pragma once

#include "flags.h"
#include "framework/math/vector.h"
#include <memory>

namespace Gorc {
namespace Content {
namespace Assets {

class LevelSurface {
public:
	int Material;
	FlagSet<SurfaceFlag> Flags;
	FlagSet<FaceTypeFlag> FaceTypeFlags;
	GeometryMode GeometryMode;
	LightMode LightMode;
	TextureMode TextureMode;
	int Adjoin;
	int AdjoinedSector;
	float ExtraLight;
	std::vector<std::tuple<int, int, float>> Vertices;
	Math::Vector<3> Normal;
	Math::Vector<2> TextureOffset;
	int CelNumber = -1;
	int AnimNumber = -1;
};

}
}
}
