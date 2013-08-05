#include "playercontroller.h"
#include "game/world/level/levelpresenter.h"
#include "game/world/level/levelmodel.h"

unsigned int Gorc::Game::World::Level::Gameplay::PlayerController::Create(const Content::Assets::Template& tpl,
		unsigned int sector_id, const Math::Vector<3>& pos, const Math::Vector<3>& orient) {
	unsigned int thing_id = CharacterController::Create(tpl, sector_id, pos, orient);
	presenter.Model->Things[thing_id].Controller = this;
	return thing_id;
}

void Gorc::Game::World::Level::Gameplay::PlayerController::Update(unsigned int thing_id, double dt) {
	CharacterController::Update(thing_id, dt);

	Thing& thing = presenter.Model->Things[thing_id];
	if(thing.AttachFlags & FlagSet<Flags::AttachFlag> { Flags::AttachFlag::AttachedToWorldSurface, Flags::AttachFlag::AttachedToThingFace }) {
		// Move in the direction of the attached face
	}
}
