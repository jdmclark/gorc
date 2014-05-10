#include "thing_controller_aspect.h"

#include "game/world/events/animation_marker.h"
#include "game/world/events/touched_surface.h"
#include "game/world/events/touched_thing.h"
#include "cog/flags/message_type.h"
#include "game/world/scripts/script_presenter.h"
#include "base/events/print.h"

using namespace gorc::game::world::aspects;

thing_controller_aspect::thing_controller_aspect(component_system &cs,
                                                 level_presenter &presenter)
    : inner_join_aspect(cs), presenter(presenter) {

    cs.bus.add_handler<events::touched_surface>(
        [this](events::touched_surface const &e) {
            on_touched_surface(e.toucher, e.touched);
        });

    cs.bus.add_handler<events::touched_thing>(
        [this](events::touched_thing const &e) {
            on_touched_thing(e.toucher, e.touched);
        });

    return;
}

void thing_controller_aspect::update(time t,
                                     entity_id thing_id,
                                     components::thing &thing) {
    thing.time_alive += static_cast<float>(t.elapsed_as_seconds());

    if(thing.timer > 0.0f && thing.time_alive >= thing.timer) {
        presenter.destroy_thing(static_cast<int>(thing_id));
    }
}

void thing_controller_aspect::on_touched_surface(entity_id thing,
                                                 int surface) {
    // Dispatch touched message to surface
    presenter.script_presenter->send_message_to_linked(cog::message_id::touched,
                                                       surface,
                                                       flags::message_type::surface,
                                                       thing,
                                                       flags::message_type::thing);
    return;
}

void thing_controller_aspect::on_touched_thing(entity_id thing,
                                               entity_id touched_thing) {
    // Dispatch touched message to thing
    presenter.script_presenter->send_message_to_linked(cog::message_id::touched,
                                                       thing,
                                                       flags::message_type::thing,
                                                       touched_thing,
                                                       flags::message_type::thing);
    return;
}
