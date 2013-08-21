#include "weaponcontroller.h"
#include "game/world/level/levelpresenter.h"
#include "game/world/level/levelmodel.h"

void Gorc::Game::World::Level::Gameplay::WeaponController::CreateControllerData(int thing_id) {
	ThingController::CreateControllerData(thing_id);

	return;
}

void Gorc::Game::World::Level::Gameplay::WeaponController::TouchedThing(int thing_id, int touched_thing_id) {
	ThingController::TouchedThing(thing_id, touched_thing_id);

	// Damage thing
	// TODO: Actual damage properties from template.
	presenter.DamageThing(touched_thing_id, 50.0f, { Flags::DamageFlag::Saber }, thing_id);

	auto& thing = presenter.Model->Things[thing_id];
	auto& touched_thing = presenter.Model->Things[touched_thing_id];
	if(touched_thing.Type == Flags::ThingType::Actor || touched_thing.Type == Flags::ThingType::Player) {
		presenter.CreateThingAtThing(thing.FleshHit, thing_id);
	}
	else {
		presenter.CreateThingAtThing(thing.Explode, thing_id);
	}

	presenter.DestroyThing(thing_id);
}

void Gorc::Game::World::Level::Gameplay::WeaponController::TouchedSurface(int thing_id, int touched_surface_id) {
	ThingController::TouchedSurface(thing_id, touched_surface_id);

	// Damage surface
	// TODO: Actual damage properties from template.
	presenter.ScriptPresenter.SendMessageToLinked(Cog::MessageId::Damaged, touched_surface_id, Flags::MessageType::Surface,
			thing_id, Flags::MessageType::Thing, 1000, static_cast<int>(Flags::DamageFlag::Saber));

	auto& thing = presenter.Model->Things[thing_id];
	presenter.CreateThingAtThing(thing.Explode, thing_id);
	presenter.DestroyThing(thing_id);
}
