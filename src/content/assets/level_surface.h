#pragma once

#include "content/flags/surfaceflag.h"
#include "content/flags/faceflag.h"
#include "content/flags/geometrymode.h"
#include "content/flags/lightmode.h"
#include "content/flags/texturemode.h"
#include "framework/math/vector.h"
#include <memory>

namespace Gorc {
namespace Content {
namespace Assets {

class LevelSurface {
public:
	int Material;
	FlagSet<Flags::SurfaceFlag> Flags;
	FlagSet<Flags::FaceFlag> FaceTypeFlags;
	Flags::GeometryMode GeometryMode;
	Flags::LightMode LightMode;
	Flags::TextureMode TextureMode;
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
