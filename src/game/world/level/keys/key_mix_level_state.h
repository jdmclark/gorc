#pragma once

#include "content/assets/animation.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Keys {

class KeyMixLevelState {
public:
	Content::Assets::Animation const* Animation = nullptr;
	double Frame;
	int Priority;
};

}
}
}
}
}
