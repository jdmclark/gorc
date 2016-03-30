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
            cs.emplace_component<components::item>(e.thing);
        }
    });

    taken_delegate =
        cs.bus.add_handler<events::taken>([&](events::taken const &e) {
        auto &player_thing = presenter.model->get_thing(e.taker);
        if(player_thing.type != flags::thing_type::Player) {
            return;
        }

        /* TODO
        presenter.script_presenter->send_message_to_linked(cog::message_id::taken,
                                                           e.taken_thing,
                                                           flags::message_type::thing,
                                                           e.taker,
                                                           flags::message_type::thing);*/
        // TODO: Don't destroy the thing. Flag it for respawn.
        presenter.destroy_thing(e.taken_thing);
    });
}
