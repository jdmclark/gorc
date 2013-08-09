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
	static void OnAnimationDestroy(Pool<PoolPtr<Animation>>&, Pool<PoolPtr<Animation>>::Element&);

public:
	Pool<PoolPtr<Animation>> Animations;

	AnimationModel();
};

}
}
}
}
}
