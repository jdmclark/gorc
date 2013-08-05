#include "playercontroller.h"
#include "game/world/level/levelpresenter.h"
#include "game/world/level/levelmodel.h"

unsigned int Gorc::Game::World::Level::Gameplay::PlayerController::Create(const Content::Assets::Template& tpl,
		unsigned int sector_id, const Math::Vector<3>& pos, const Math::Vector<3>& orient) {
	unsigned int thing_id = CharacterController::Create(tpl, sector_id, pos, orient);
	presenter.Model->Things[thing_id].Controller = this;
	return thing_id;
}
