#pragma once

#include "animation_frame.h"
#include <vector>

namespace Gorc {
namespace Content {
namespace Assets {

class AnimationNode {
public:
	std::vector<AnimationFrame> Frames;
};

}
}
}
