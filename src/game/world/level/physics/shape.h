#pragma once

#include <tuple>
#include "framework/math/vector.h"

namespace gorc {
namespace game {
namespace world {
namespace level {
namespace physics {

using segment = std::tuple<vector<3>, vector<3>>;
using sphere = std::tuple<vector<3>, float>;
using swept_sphere = std::tuple<segment, float>;

}
}
}
}
}
