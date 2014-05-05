#include "thing_controller.h"
#include "game/world/level_presenter.h"
#include "game/world/level_model.h"
#include "game/constants.h"
#include "game/world/physics/query.h"
#include "game/world/scripts/script_presenter.h"
#include "game/world/events/animation_marker.h"

gorc::game::world::gameplay::thing_controller::thing_controller(level_presenter& presenter)
    : presenter(presenter) {
    return;
}

gorc::game::world::gameplay::thing_controller::~thing_controller() {
    return;
}

void gorc::game::world::gameplay::thing_controller::update(int, double) {
    return;
}

void gorc::game::world::gameplay::thing_controller::create_controller_data(int thing_id) {
    auto& new_thing = presenter.model->get_thing(thing_id);
    new_thing.time_alive = 0.0f;
}

void gorc::game::world::gameplay::thing_controller::remove_controller_data(int) {
    return;
}

void gorc::game::world::gameplay::thing_controller::taken(int, int) {
    return;
}

void gorc::game::world::gameplay::thing_controller::touched_thing(int thing_id, int touched_thing_id) {
    // Dispatch touched cog message.
    presenter.script_presenter->send_message_to_linked(cog::message_id::touched,
            thing_id, flags::message_type::thing,
            touched_thing_id, flags::message_type::thing);
    return;
}

void gorc::game::world::gameplay::thing_controller::touched_surface(int thing_id, int touched_surface_id) {
    // Dispatch touched message to surface.
    presenter.script_presenter->send_message_to_linked(cog::message_id::touched,
            touched_surface_id, flags::message_type::surface,
            thing_id, flags::message_type::thing);
    return;
}

void gorc::game::world::gameplay::thing_controller::handle_animation_marker(int, flags::key_marker_type) {
    return;
}
