#pragma once

#include "base/utility/inner_join_aspect.h"
#include "game/world/components/thing.h"
#include "game/world/sounds/components/thing_sound.h"

namespace gorc {
namespace game {
namespace world {
namespace sounds {
namespace aspects {

class thing_sound_aspect : public inner_join_aspect<components::thing_sound, world::components::thing> {
public:
    explicit thing_sound_aspect(component_system &cs);

    void update(time, entity_id, components::thing_sound&, world::components::thing&);
};

}
}
}
}
}
