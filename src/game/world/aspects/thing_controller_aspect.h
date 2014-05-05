#pragma once

#include "base/utility/inner_join_aspect.h"
#include "game/world/components/thing.h"
#include "game/world/level_presenter.h"

namespace gorc {
namespace game {
namespace world {
namespace aspects {

class thing_controller_aspect : public inner_join_aspect<components::thing> {
private:
    level_presenter& presenter;
public:
    thing_controller_aspect(component_system& cs, level_presenter& presenter);

    void update(time, entity_id, components::thing&);
};

}
}
}
}
