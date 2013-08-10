#include "playercontroller.h"
#include "game/world/level/levelpresenter.h"
#include "game/world/level/levelmodel.h"

void Gorc::Game::World::Level::Gameplay::PlayerController::Update(int thing_id, double dt) {
	CharacterController::Update(thing_id, dt);

	Thing& thing = presenter.Model->Things[thing_id];
	if(thing.AttachFlags & FlagSet<Flags::AttachFlag> { Flags::AttachFlag::AttachedToWorldSurface, Flags::AttachFlag::AttachedToThingFace }) {
		// Move in the direction of the attached face
	}
}
