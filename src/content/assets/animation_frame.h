#pragma once

#include "framework/math/vector.h"

namespace Gorc {
namespace Content {
namespace Assets {

class AnimationFrame {
public:
	int Frame;
	Math::Vector<3> Position;
	Math::Vector<3> Orientation;
	Math::Vector<3> DeltaPosition;
	Math::Vector<3> DeltaOrientation;
};

}
}
}
