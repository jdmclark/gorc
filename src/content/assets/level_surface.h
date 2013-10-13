#pragma once

#include "content/flags/surface_flag.h"
#include "content/flags/face_flag.h"
#include "content/flags/geometry_mode.h"
#include "content/flags/light_mode.h"
#include "content/flags/texture_mode.h"
#include "framework/math/vector.h"
#include "framework/flag_set.h"
#include <memory>
#include <vector>
#include <tuple>

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
	Math::Vector<3> Thrust;
};

}
}
}
