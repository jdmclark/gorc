#pragma once

#include "animation.h"
#include "framework/pool.h"
#include <vector>
#include <memory>

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Animations {

class AnimationModel {
private:
	static void OnAnimationDestroy(Pool<std::unique_ptr<Animation>>&, unsigned int, std::unique_ptr<Animation>& anim);

public:
	Pool<std::unique_ptr<Animation>> Animations;

	AnimationModel();
};

}
}
}
}
}
