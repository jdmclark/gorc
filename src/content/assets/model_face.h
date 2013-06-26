#pragma once

#include "framework/flagset.h"
#include "flags.h"

#include <vector>
#include <tuple>
#include <btBulletDynamicsCommon.h>

namespace Gorc {
namespace Content {
namespace Assets {

class ModelFace {
public:
	size_t Material;
	FlagSet<FaceTypeFlag> Type;
	GeometryMode Geo;
	LightMode Light;
	TextureMode Tex;
	float ExtraLight;
	std::vector<std::tuple<size_t, size_t>> Vertices;
	Math::Vector<3> Normal;

	std::unique_ptr<btConvexHullShape> Shape;
};

}
}
}
