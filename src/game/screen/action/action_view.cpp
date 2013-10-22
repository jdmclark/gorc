#include "action_view.h"
#include "action_presenter.h"

void gorc::game::screen::action::action_view::update(double dt) {
	if(currentPresenter) {
		currentPresenter->update(dt);
	}
}

void gorc::game::screen::action::action_view::draw(double dt, const box<2, int>& view_size) {
	// TODO
}
