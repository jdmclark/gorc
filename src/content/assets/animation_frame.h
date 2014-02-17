#pragma once

#include "framework/math/vector.h"

namespace gorc {
namespace content {
namespace assets {

class animation_frame {
public:
    int frame;
    vector<3> position;
    vector<3> orientation;
    vector<3> delta_position;
    vector<3> delta_orientation;
};

}
}
}
