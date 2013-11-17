#include "weapon_controller.h"
#include "game/world/level_presenter.h"
#include "game/world/level_model.h"

void gorc::game::world::gameplay::weapon_controller::touched_thing(int thing_id, int touched_thing_id) {
	thing_controller::touched_thing(thing_id, touched_thing_id);

	// Damage thing
	// TODO: Actual damage properties from template.
	presenter.damage_thing(touched_thing_id, 50.0f, { flags::DamageFlag::Saber }, thing_id);

	auto& thing = presenter.model->things[thing_id];
	auto& touched_thing = presenter.model->things[touched_thing_id];
	if(touched_thing.type == flags::thing_type::Actor || touched_thing.type == flags::thing_type::Player) {
		presenter.create_thing_at_thing(thing.flesh_hit, thing_id);
	}
	else {
		presenter.create_thing_at_thing(thing.explode, thing_id);
	}

	presenter.destroy_thing(thing_id);
}

void gorc::game::world::gameplay::weapon_controller::touched_surface(int thing_id, int touched_surface_id) {
	thing_controller::touched_surface(thing_id, touched_surface_id);

	// Damage surface
	// TODO: Actual damage properties from template.
	presenter.script_presenter.send_message_to_linked(cog::message_id::damaged, touched_surface_id, flags::message_type::surface,
			thing_id, flags::message_type::thing, 1000, static_cast<int>(flags::DamageFlag::Saber));

	auto& thing = presenter.model->things[thing_id];
	presenter.create_thing_at_thing(thing.explode, thing_id);
	presenter.destroy_thing(thing_id);
}
