#include "item_controller_aspect.h"
#include "game/world/events/taken.h"
#include "game/world/scripts/script_presenter.h"
#include "game/world/level_model.h"

using gorc::game::world::aspects::item_controller_aspect;

item_controller_aspect::item_controller_aspect(component_system &cs,
                                               level_presenter &presenter)
    : inner_join_aspect(cs), presenter(presenter) {
    cs.bus.add_handler<events::taken>([&](events::taken const &e) {
        auto &player_thing = presenter.model->get_thing(e.taker);
        if(player_thing.type != flags::thing_type::Player) {
            return;
        }

        presenter.script_presenter->send_message_to_linked(cog::message_id::taken,
                                                           e.taken_thing,
                                                           flags::message_type::thing,
                                                           e.taker,
                                                           flags::message_type::thing);
        // TODO: Don't destroy the thing. Flag it for respawn.
        presenter.destroy_thing(e.taken_thing);
    });
}
