#pragma once

#include "thing_controller.h"

namespace gorc {
namespace game {
namespace world {

namespace components {
class thing;
}

namespace gameplay {

class cog_controller : public thing_controller {
public:
    using thing_controller::thing_controller;

    virtual void update(int thing_id, double dt) override;
};

}
}
}
}
