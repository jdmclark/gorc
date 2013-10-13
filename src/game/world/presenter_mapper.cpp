#include "presenter_mapper.h"
#include "game/world/nothing/nothing_presenter.h"
#include "game/world/level/level_presenter.h"

Gorc::Game::World::PresenterMapper::PresenterMapperPlaceVisitor::PresenterMapperPlaceVisitor(Gorc::Game::Components& components)
	: components(components) {
	return;
}

void Gorc::Game::World::PresenterMapper::PresenterMapperPlaceVisitor::VisitNothingPlace(const Nothing::NothingPlace& place) {
	presenter = std::unique_ptr<Gorc::Place::Presenter>(new Nothing::NothingPresenter(components));
}

void Gorc::Game::World::PresenterMapper::PresenterMapperPlaceVisitor::VisitLevelPlace(const Level::LevelPlace& place) {
	presenter = std::unique_ptr<Gorc::Place::Presenter>(new Level::LevelPresenter(components, place));
}

std::unique_ptr<Gorc::Place::Presenter> Gorc::Game::World::PresenterMapper::GetPresenter(const Place& place) {
	PresenterMapperPlaceVisitor v(*components);
	place.Accept(v);
	return std::move(v.presenter);
}
