#pragma once

#include "libold/base/utility/inner_join_aspect.hpp"
#include "game/world/level_presenter.hpp"
#include "game/world/components/item.hpp"
#include "game/world/components/thing.hpp"

namespace gorc {
namespace game {
namespace world {
namespace aspects {

class item_controller_aspect : public inner_join_aspect<components::item, components::thing> {
private:
    level_presenter &presenter;

    maybe<scoped_delegate> created_delegate;
    maybe<scoped_delegate> taken_delegate;

public:
    item_controller_aspect(component_system&, level_presenter&);
};

}
}
}
}
