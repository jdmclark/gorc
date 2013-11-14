#include "action_view.h"
#include "action_presenter.h"
#include <iostream>

gorc::game::action::action_view::action_view() {
	return;
}

gorc::maybe<gorc::input::input_adapter*> gorc::game::action::action_view::get_input_adapter() {
	return presenter.if_set([](action_presenter& presenter) {
		return make_maybe(&presenter.get_input_adapter());
	}, maybe<input::input_adapter*>());
}
