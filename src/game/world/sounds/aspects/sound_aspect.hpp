#pragma once

#include "libold/base/utility/inner_join_aspect.hpp"
#include "game/world/sounds/components/sound.hpp"
#include "game/world/level_model.hpp"

namespace gorc {
namespace game {
namespace world {
namespace sounds {
namespace aspects {

class sound_aspect : public inner_join_aspect<components::sound> {
private:
    level_model &model;
    maybe<scoped_delegate> destroyed_delegate;

public:
    sound_aspect(component_system&, level_model&);

    void update(gorc::time, entity_id, components::sound&);
};

}
}
}
}
}
