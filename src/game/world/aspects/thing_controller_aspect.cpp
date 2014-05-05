#include "thing_controller_aspect.h"

#include "game/world/events/animation_marker.h"
#include "base/events/print.h"

gorc::game::world::aspects::thing_controller_aspect::thing_controller_aspect(component_system& cs, level_presenter& presenter)
    : inner_join_aspect(cs), presenter(presenter) {
    return;
}

void gorc::game::world::aspects::thing_controller_aspect::update(time t, entity_id thing_id, components::thing& thing) {
    thing.time_alive += static_cast<float>(t.elapsed_as_seconds());

    if(thing.timer > 0.0f && thing.time_alive >= thing.timer) {
        presenter.destroy_thing(static_cast<int>(thing_id));
    }
}
