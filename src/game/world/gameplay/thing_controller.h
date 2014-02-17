#pragma once

#include "framework/math/vector.h"
#include "content/flags/key_marker_type.h"
#include "content/assets/model.h"
#include "framework/math/matrix.h"
#include "game/world/physics/shape.h"
#include <vector>
#include <set>
#include <stack>

namespace gorc {

namespace content {
namespace assets {
class thing_template;
}
}

namespace game {
namespace world {

class level_presenter;
class thing;

namespace gameplay {

class thing_controller {
protected:
    level_presenter& presenter;

public:
    thing_controller(level_presenter& presenter);
    virtual ~thing_controller() = 0;

    virtual void update(int thing_id, double dt);
    virtual void remove_controller_data(int thing_id);
    virtual void create_controller_data(int thing_id);
    virtual void handle_animation_marker(int thing_id, flags::key_marker_type marker);

    virtual void taken(int thing_id, int player_id);
    virtual void touched_thing(int thing_id, int touched_thing_id);
    virtual void touched_surface(int thing_id, int touched_surface_id);
};

}
}
}
}
