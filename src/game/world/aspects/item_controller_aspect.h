#pragma once

#include "base/utility/inner_join_aspect.h"
#include "game/world/level_presenter.h"
#include "game/world/components/item.h"
#include "game/world/components/thing.h"

namespace gorc {
namespace game {
namespace world {
namespace aspects {

class item_controller_aspect : public inner_join_aspect<components::item, components::thing> {
private:
    level_presenter &presenter;

public:
    item_controller_aspect(component_system&, level_presenter&);
};

}
}
}
}
