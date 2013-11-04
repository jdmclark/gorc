#pragma once

#include "content/assets/level_surface.h"
#include "animations/animation.h"
#include "framework/utility/pool.h"
#include "physics/object_data.h"

namespace gorc {
namespace game {
namespace world {
namespace level {

class surface : public content::assets::level_surface {
public:
	physics::surface_object_data object_data;

	int cel_number = -1;
	int anim_number = -1;

	inline const physics::surface_object_data& get_object_data() const {
		return object_data;
	}

	surface(const content::assets::level_surface& surf);
};

}
}
}
}
