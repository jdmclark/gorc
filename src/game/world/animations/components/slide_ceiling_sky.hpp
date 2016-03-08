#pragma once

#include "math/vector.hpp"

namespace gorc {
namespace game {
namespace world {
namespace animations {
namespace components {

class slide_ceiling_sky {
public:
    #include "slide_ceiling_sky.uid"

    vector<2> speed;

    slide_ceiling_sky(const vector<2>& speed);
};

}
}
}
}
}
