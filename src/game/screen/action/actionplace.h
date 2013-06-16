#pragma once

#include "game/screen/place.h"
#include <string>

namespace Gorc {
namespace Game {
namespace Screen {
namespace Action {

class ActionPlace : public Place {
public:
	void Accept(PlaceVisitor& v) const;
};

}
}
}
}
