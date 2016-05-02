#include "thing_controller_aspect.hpp"

#include "game/world/events/animation_marker.hpp"
#include "game/world/events/touched_surface.hpp"
#include "game/world/events/touched_thing.hpp"
#include "libold/base/events/print.hpp"
#include "game/world/level_model.hpp"

using namespace gorc::game::world::aspects;

thing_controller_aspect::thing_controller_aspect(entity_component_system<thing_id> &cs,
                                                 level_presenter &presenter)
    : inner_join_aspect(cs), presenter(presenter) {

    touched_surface_delegate =
        cs.bus.add_handler<events::touched_surface>(
        [this](events::touched_surface const &e) {
            on_touched_surface(e.toucher, e.touched);
        });

    touched_thing_delegate =
        cs.bus.add_handler<events::touched_thing>(
        [this](events::touched_thing const &e) {
            on_touched_thing(e.toucher, e.touched);
        });

    return;
}

void thing_controller_aspect::update(time_delta t,
                                     thing_id tid,
                                     components::thing &thing) {
    thing.time_alive += static_cast<float>(t.count());

    if(thing.timer > 0.0f && thing.time_alive >= thing.timer) {
        presenter.destroy_thing(tid);
    }
}

void thing_controller_aspect::on_touched_surface(thing_id thing,
                                                 surface_id surface) {
    // Dispatch touched message to surface
    presenter.model->send_to_linked(
            cog::message_type::touched,
            /* sender */ surface,
            /* source */ thing);
    return;
}

void thing_controller_aspect::on_touched_thing(thing_id thing,
                                               thing_id touched_thing) {
    // Dispatch touched message to thing
    presenter.model->send_to_linked(
            cog::message_type::touched,
            /* sender */ touched_thing,
            /* source */ thing);
    return;
}
