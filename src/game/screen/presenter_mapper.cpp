#include "presenter_mapper.h"
#include "game/screen/action/action_presenter.h"

Gorc::Game::Screen::PresenterMapper::PresenterMapperPlaceVisitor::PresenterMapperPlaceVisitor(Gorc::Game::Components& components)
	: components(components) {
	return;
}

void Gorc::Game::Screen::PresenterMapper::PresenterMapperPlaceVisitor::VisitActionPlace(const Action::ActionPlace& place) {
	presenter = std::unique_ptr<Gorc::Place::Presenter>(new Action::ActionPresenter(components, place));
}

std::unique_ptr<Gorc::Place::Presenter> Gorc::Game::Screen::PresenterMapper::GetPresenter(const Place& place) {
	PresenterMapperPlaceVisitor v(*components);
	place.Accept(v);
	return std::move(v.presenter);
}
