#pragma once

#include "content/assets/animation.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Keys {

class KeyMix {
public:
	bool Playing = false;
	Content::Assets::Animation const* Animation = nullptr;
	double AnimationTime = 0.0f;
};

}
}
}
}
}
