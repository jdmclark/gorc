#pragma once

#include "libold/base/utility/inner_join_aspect.hpp"
#include "game/world/components/thing.hpp"
#include "game/world/sounds/components/thing_sound.hpp"

namespace gorc {
namespace game {
namespace world {
namespace sounds {
namespace aspects {

class thing_sound_aspect : public inner_join_aspect<components::thing_sound, world::components::thing> {
public:
    explicit thing_sound_aspect(entity_component_system &cs);

    void update(time_delta, thing_id, components::thing_sound&, world::components::thing&);
};

}
}
}
}
}
