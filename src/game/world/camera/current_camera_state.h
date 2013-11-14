#pragma once

#include "framework/math/vector.h"

namespace gorc {
namespace game {
namespace world {
namespace camera {

class current_camera_state {
public:
	vector<3> position;
	vector<3> look;
	vector<3> up;
	int containing_sector;
	int focus_not_drawn_thing;
};

}
}
}
}
