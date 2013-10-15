#pragma once

#include "game/screen/place.h"
#include <string>

namespace gorc {
namespace game {
namespace screen {
namespace action {

class action_place : public place {
public:
	void accept(place_visitor& v) const;
};

}
}
}
}
