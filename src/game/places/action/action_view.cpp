#include "action_view.h"
#include "action_presenter.h"
#include <iostream>

gorc::game::action::action_view::action_view() {
	return;
}

gorc::maybe<gorc::input::input_adapter*> gorc::game::action::action_view::get_input_adapter() {
	maybe<input::input_adapter*> rv;
	if_set(presenter, then_do, [&rv](action_presenter& p) {
		rv = &p.get_input_adapter();
	},
	else_do, [&rv] {
		rv = nothing;
	});

	return rv;
}
