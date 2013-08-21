#include "thingcontroller.h"
#include "game/world/level/levelpresenter.h"
#include "game/world/level/levelmodel.h"
#include "game/constants.h"

Gorc::Game::World::Level::Gameplay::ThingController::ThingController(LevelPresenter& presenter)
	: presenter(presenter) {
	return;
}

Gorc::Game::World::Level::Gameplay::ThingController::~ThingController() {
	return;
}

void Gorc::Game::World::Level::Gameplay::ThingController::Update(int thing_id, double dt) {
	auto& thing = presenter.Model->Things[thing_id];
	thing.TimeAlive += dt;

	if(thing.Timer && thing.TimeAlive >= thing.Timer) {
		presenter.DestroyThing(thing_id);
	}
}

void Gorc::Game::World::Level::Gameplay::ThingController::HandleAnimationMarker(int thing_id, Flags::KeyMarkerType marker) {
	return;
}

void Gorc::Game::World::Level::Gameplay::ThingController::CreateControllerData(int thing_id) {
	auto& new_thing = presenter.Model->Things[thing_id];
	new_thing.TimeAlive = 0.0f;
}

void Gorc::Game::World::Level::Gameplay::ThingController::RemoveControllerData(int thing_id) {
	return;
}

void Gorc::Game::World::Level::Gameplay::ThingController::Taken(int thing_id, int player_id) {
	return;
}

void Gorc::Game::World::Level::Gameplay::ThingController::TouchedThing(int thing_id, int touched_thing_id) {
	// Dispatch touched cog message.
	presenter.ScriptPresenter.SendMessageToLinked(Cog::MessageId::Touched,
			thing_id, Flags::MessageType::Thing,
			touched_thing_id, Flags::MessageType::Thing);
	return;
}

void Gorc::Game::World::Level::Gameplay::ThingController::TouchedSurface(int thing_id, int touched_surface_id) {
	return;
}
