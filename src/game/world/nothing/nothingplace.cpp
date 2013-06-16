#include "nothingplace.h"

void Gorc::Game::World::Nothing::NothingPlace::Accept(PlaceVisitor& v) const {
	v.VisitNothingPlace(*this);
}
