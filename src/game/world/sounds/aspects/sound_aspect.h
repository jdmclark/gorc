#pragma once

#include "base/utility/inner_join_aspect.h"
#include "game/world/sounds/components/sound.h"
#include "game/world/level_model.h"

namespace gorc {
namespace game {
namespace world {
namespace sounds {
namespace aspects {

class sound_aspect : public inner_join_aspect<components::sound> {
private:
    level_model &model;

public:
    sound_aspect(component_system&, level_model&);

    void update(time, entity_id, components::sound&);
};

}
}
}
}
}
