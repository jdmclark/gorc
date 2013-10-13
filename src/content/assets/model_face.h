#pragma once

#include "framework/flag_set.h"
#include "content/flags/face_flag.h"
#include "content/flags/geometry_mode.h"
#include "content/flags/light_mode.h"
#include "content/flags/texture_mode.h"

#include <vector>
#include <tuple>

namespace Gorc {
namespace Content {
namespace Assets {

class ModelFace {
public:
	int Material;
	FlagSet<Flags::FaceFlag> Type;
	Flags::GeometryMode Geo;
	Flags::LightMode Light;
	Flags::TextureMode Tex;
	float ExtraLight;
	std::vector<std::tuple<size_t, size_t>> Vertices;
	Math::Vector<3> Normal;
};

}
}
}
