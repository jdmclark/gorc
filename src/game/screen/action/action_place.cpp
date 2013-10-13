#include "action_place.h"

void Gorc::Game::Screen::Action::ActionPlace::Accept(PlaceVisitor& v) const {
	v.VisitActionPlace(*this);
}
