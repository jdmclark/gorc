#include "thing_controller.h"
#include "game/world/level/level_presenter.h"
#include "game/world/level/level_model.h"
#include "game/constants.h"

gorc::game::world::level::gameplay::thing_controller::thing_controller(level_presenter& presenter)
	: presenter(presenter) {
	return;
}

gorc::game::world::level::gameplay::thing_controller::~thing_controller() {
	return;
}

void gorc::game::world::level::gameplay::thing_controller::update(int thing_id, double dt) {
	auto& thing = presenter.model->things[thing_id];
	thing.time_alive += dt;

	if(thing.timer && thing.time_alive >= thing.timer) {
		presenter.destroy_thing(thing_id);
	}
}

void gorc::game::world::level::gameplay::thing_controller::handle_animation_marker(int thing_id, flags::key_marker_type marker) {
	return;
}

void gorc::game::world::level::gameplay::thing_controller::create_controller_data(int thing_id) {
	auto& new_thing = presenter.model->things[thing_id];
	new_thing.time_alive = 0.0f;
}

void gorc::game::world::level::gameplay::thing_controller::remove_controller_data(int thing_id) {
	return;
}

void gorc::game::world::level::gameplay::thing_controller::taken(int thing_id, int player_id) {
	return;
}

void gorc::game::world::level::gameplay::thing_controller::touched_thing(int thing_id, int touched_thing_id) {
	// Dispatch touched cog message.
	presenter.script_presenter.send_message_to_linked(cog::message_id::touched,
			thing_id, flags::message_type::thing,
			touched_thing_id, flags::message_type::thing);
	return;
}

void gorc::game::world::level::gameplay::thing_controller::touched_surface(int thing_id, int touched_surface_id) {
	return;
}
