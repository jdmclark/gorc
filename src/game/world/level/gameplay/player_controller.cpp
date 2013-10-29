#include "player_controller.h"
#include "game/world/level/level_presenter.h"
#include "game/world/level/level_model.h"
#include "game/world/level/physics/query.h"

void gorc::game::world::level::gameplay::player_controller::update(int thing_id, double dt) {
	character_controller::update(thing_id, dt);
}
