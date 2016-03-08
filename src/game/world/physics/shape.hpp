#pragma once

#include <tuple>
#include "math/vector.hpp"

namespace gorc {
namespace game {
namespace world {
namespace physics {

using segment = std::tuple<vector<3>, vector<3>>;

class sphere {
public:
    vector<3> position;
    float radius;

    sphere() = default;
    sphere(const vector<3>& position, float radius);
};

class swept_sphere {
public:
    segment position;
    float radius;

    swept_sphere() = default;
    swept_sphere(const segment& position, float radius);
};

}
}
}
}
