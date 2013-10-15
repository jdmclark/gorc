#include "nothing_place.h"

void gorc::game::world::nothing::nothing_place::accept(place_visitor& v) const {
	v.visit_nothing_place(*this);
}
