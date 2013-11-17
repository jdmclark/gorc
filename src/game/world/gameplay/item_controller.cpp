#include "item_controller.h"
#include "game/world/level_presenter.h"
#include "game/world/level_model.h"

void gorc::game::world::gameplay::item_controller::taken(int thing_id, int player_id) {
	auto& player_thing = presenter.model->things[player_id];
	if(player_thing.type != flags::thing_type::Player) {
		return;
	}

	presenter.script_presenter.send_message_to_linked(cog::message_id::taken,
			thing_id, flags::message_type::thing,
			player_id, flags::message_type::thing);

	// TODO: Don't destroy the thing. Flag it for respawn.
	presenter.destroy_thing(thing_id);
}
