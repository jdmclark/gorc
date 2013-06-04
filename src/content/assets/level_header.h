#pragma once

#include "framework/math/vector.h"
#include <tuple>

namespace Gorc {
namespace Content {
namespace Assets {

class LevelHeader {
public:
	int Version = 1;
	float WorldGravity = 4.0f;
	float CeilingSkyZ = 20.0f;
	float HorizonDistance = 100.0f;
	float HorizonPixelsPerRev = 768.0f;
	Math::Vector<2> HorizonSkyOffset = Math::Zero<2>();
	Math::Vector<2> CeilingSkyOffset = Math::Zero<2>();
	std::tuple<float, float, float, float> MipMapDistances = std::make_tuple(1.0f, 2.0f, 3.0f, 4.0f);
	std::tuple<float, float, float, float> LODDistances = std::make_tuple(0.3f, 0.6f, 0.9f, 1.2f);
	float PerspectiveDistance;
	float GouraudDistance;
};

}
}
}
