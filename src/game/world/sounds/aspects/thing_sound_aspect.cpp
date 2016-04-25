#include "thing_sound_aspect.hpp"
#include "game/world/sounds/components/sound.hpp"

gorc::game::world::sounds::aspects::thing_sound_aspect::thing_sound_aspect(entity_component_system &cs)
    : inner_join_aspect(cs) {
    return;
}

void gorc::game::world::sounds::aspects::thing_sound_aspect::update(time_delta,
                                                                    thing_id,
                                                                    components::thing_sound &ts,
                                                                    world::components::thing &thing) {
    for(auto &sound : cs.find_component<components::sound>(ts.sound)) {
        sound.second.position = thing.position;
        sound.second.internal_sound.setPosition(get<0>(thing.position),
                                                get<2>(thing.position),
                                                -get<1>(thing.position));
    }
}
