#pragma once

#include "game/world/place.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Nothing {

class NothingPlace : public Place {
public:
	void Accept(PlaceVisitor& v) const;
};

}
}
}
}
