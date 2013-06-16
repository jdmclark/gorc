#include "actionview.h"
#include "actionpresenter.h"

void Gorc::Game::Screen::Action::ActionView::Update(double dt) {
	if(currentPresenter) {
		currentPresenter->Update(dt);
	}
}

void Gorc::Game::Screen::Action::ActionView::Draw(double dt, const Math::Box<2, unsigned int>& view_size) {
	// TODO
}
