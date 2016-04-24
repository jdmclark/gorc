#pragma once

#include "content/id.hpp"
#include "math/vector.hpp"

namespace gorc {
namespace game {
namespace world {
namespace animations {
namespace components {

class slide_surface {
public:
    #include "slide_surface.uid"

    surface_id surface;
    vector<3> direction;

    vector<3> sb0, sb1;
    vector<2> tb0, tb1;

    slide_surface(surface_id surface, const vector<3>& direction, const vector<3>& sb0, const vector<3>& sb1,
            const vector<2>& tb0, const vector<2>& tb1);
};

}
}
}
}
}
