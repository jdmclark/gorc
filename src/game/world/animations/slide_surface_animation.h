#pragma once

#include "animation.h"
#include "framework/math/vector.h"

namespace gorc {
namespace game {
namespace world {

class level_model;

namespace animations {

class slide_surface_animation : public animation {
private:
    level_model& model;
    unsigned int surface;
    vector<3> direction;

    vector<3> sb0, sb1;
    vector<2> tb0, tb1;

public:
    slide_surface_animation(level_model& model, unsigned int surface, const vector<3>& direction, int anim_num);

    void update(double dt);
    void stop();
};

}
}
}
}
