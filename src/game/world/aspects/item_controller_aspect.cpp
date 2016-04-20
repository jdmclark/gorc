#include "item_controller_aspect.hpp"
#include "game/world/events/taken.hpp"
#include "game/world/level_model.hpp"
#include "game/world/events/thing_created.hpp"

using gorc::game::world::aspects::item_controller_aspect;

item_controller_aspect::item_controller_aspect(component_system &cs,
                                               level_presenter &presenter)
    : inner_join_aspect(cs), presenter(presenter) {

    created_delegate =
        cs.bus.add_handler<events::thing_created>([&](events::thing_created const &e) {
        if(e.tpl.type == flags::thing_type::Item) {
            cs.emplace_component<components::item>(entity_id(e.thing));
        }
    });

    taken_delegate =
        cs.bus.add_handler<events::taken>([&](events::taken const &e) {
        auto &player_thing = presenter.model->get_thing(e.taker);
        if(player_thing.type != flags::thing_type::Player) {
            return;
        }

        presenter.model->send_to_linked(
                cog::message_type::taken,
                /* sender */ thing_id(e.taken_thing),
                /* source */ thing_id(e.taker));

        // TODO: Don't destroy the thing. Flag it for respawn.
        presenter.destroy_thing(e.taken_thing);
    });
}
