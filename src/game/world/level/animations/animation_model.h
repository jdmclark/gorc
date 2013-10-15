#pragma once

#include "animation.h"
#include "framework/pool.h"
#include <vector>
#include <memory>

namespace gorc {
namespace game {
namespace world {
namespace level {
namespace animations {

class animation_model {
private:
	static void on_animation_destroy(pool<pool_ptr<animation>>&, pool<pool_ptr<animation>>::element&);

public:
	pool<pool_ptr<animation>> animations;

	animation_model();
};

}
}
}
}
}
