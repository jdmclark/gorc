#pragma once

#include "framework/flagset.h"
#include "content/flags/faceflag.h"
#include "content/flags/geometrymode.h"
#include "content/flags/lightmode.h"
#include "content/flags/texturemode.h"

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
