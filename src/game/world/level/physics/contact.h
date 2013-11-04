#pragma once

#include "object_data.h"
#include "framework/math/vector.h"

namespace gorc {
namespace game {
namespace world {
namespace level {
namespace physics {

class contact {
public:
	vector<3> normal;
	vector<3> velocity;

	contact(const vector<3>& normal, const vector<3>& velocity);
};

}
}
}
}
}
