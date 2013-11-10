#include "action_place.h"

void gorc::game::screen::action::action_place::accept(place_visitor& v) const {
	v.visit_action_place(*this);
}
