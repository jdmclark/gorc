#pragma once

#include "game/world/place.h"

namespace gorc {
namespace game {
namespace world {
namespace nothing {

class nothing_place : public place {
public:
	void accept(place_visitor& v) const;
};

}
}
}
}
