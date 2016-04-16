#pragma once

#include "object_data.hpp"
#include "math/vector.hpp"
#include "utility/maybe.hpp"
#include "content/id.hpp"

namespace gorc {
namespace game {
namespace world {
namespace physics {

class contact {
public:
    vector<3> position;
    vector<3> normal;
    vector<3> velocity;
    maybe<thing_id> contact_thing_id;
    maybe<surface_id> contact_surface_id;

    contact() = default;
    contact(const vector<3>& position, const vector<3>& normal, const vector<3>& velocity);
};

}
}
}
}
