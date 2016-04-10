#include "thing_controller_aspect.hpp"

#include "game/world/events/animation_marker.hpp"
#include "game/world/events/touched_surface.hpp"
#include "game/world/events/touched_thing.hpp"
#include "libold/base/events/print.hpp"
#include "game/world/level_model.hpp"

using namespace gorc::game::world::aspects;

thing_controller_aspect::thing_controller_aspect(component_system &cs,
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

void thing_controller_aspect::update(gorc::time t,
                                     entity_id thing_id,
                                     components::thing &thing) {
    thing.time_alive += static_cast<float>(t.elapsed_as_seconds());

    if(thing.timer > 0.0f && thing.time_alive >= thing.timer) {
        presenter.destroy_thing(static_cast<int>(thing_id));
    }
}

void thing_controller_aspect::on_touched_surface(int thing,
                                                 int surface) {
    // Dispatch touched message to surface
    presenter.model->script_model.send_to_linked(
            cog::message_type::touched,
            /* sender */ surface_id(surface),
            /* source */ thing_id(thing),
            /* source type */ presenter.model->get_thing_source_type(thing));
    return;
}

void thing_controller_aspect::on_touched_thing(int thing,
                                               int touched_thing) {
    // Dispatch touched message to thing
    presenter.model->script_model.send_to_linked(
            cog::message_type::touched,
            /* sender */ thing_id(touched_thing),
            /* source */ thing_id(thing),
            /* source type */ presenter.model->get_thing_source_type(thing));
    return;
}
