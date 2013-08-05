#include "itemcontroller.h"
#include "game/world/level/levelpresenter.h"
#include "game/world/level/levelmodel.h"

void Gorc::Game::World::Level::Gameplay::ItemController::Update(unsigned int thing_id, double dt) {
	return;
}

unsigned int Gorc::Game::World::Level::Gameplay::ItemController::Create(const Content::Assets::Template& tpl,
		unsigned int sector_id, const Math::Vector<3>& pos, const Math::Vector<3>& orient) {
	auto new_thing_tuple = presenter.Model->Things.Create();
	auto& new_thing = *std::get<0>(new_thing_tuple);
	new_thing = tpl;

	new_thing.Position = pos;
	new_thing.Orientation = orient;
	new_thing.Controller = this;

	return std::get<1>(new_thing_tuple);
}
