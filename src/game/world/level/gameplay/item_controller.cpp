#include "item_controller.h"
#include "game/world/level/level_presenter.h"
#include "game/world/level/level_model.h"

void Gorc::Game::World::Level::Gameplay::ItemController::CreateControllerData(int thing_id) {
	ThingController::CreateControllerData(thing_id);

	return;
}

void Gorc::Game::World::Level::Gameplay::ItemController::Taken(int thing_id, int player_id) {
	auto& player_thing = presenter.Model->Things[player_id];
	if(player_thing.Type != Flags::ThingType::Player) {
		return;
	}

	presenter.ScriptPresenter.SendMessageToLinked(Cog::MessageId::Taken,
			thing_id, Flags::MessageType::Thing,
			player_id, Flags::MessageType::Thing);

	// TODO: Don't destroy the thing. Flag it for respawn.
	presenter.DestroyThing(thing_id);
}
