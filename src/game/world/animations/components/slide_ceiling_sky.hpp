#pragma once

#include "math/vector.hpp"
#include "utility/uid.hpp"

namespace gorc {
namespace game {
namespace world {
namespace animations {
namespace components {

class slide_ceiling_sky {
public:
    uid(573406198);

    vector<2> speed;

    slide_ceiling_sky(const vector<2>& speed);
};

}
}
}
}
}
