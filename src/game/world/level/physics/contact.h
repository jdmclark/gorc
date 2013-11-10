#pragma once

#include "object_data.h"
#include "framework/math/vector.h"
#include "framework/utility/maybe.h"

namespace gorc {
namespace game {
namespace world {
namespace level {
namespace physics {

class contact {
public:
	vector<3> position;
	vector<3> normal;
	vector<3> velocity;
	maybe<int> contact_thing_id;

	contact(const vector<3>& position, const vector<3>& normal, const vector<3>& velocity);
	contact(const vector<3>& position, const vector<3>& normal, const vector<3>& velocity, int contact_thing_id);
};

}
}
}
}
}
