#pragma once

#include "game/view.h"

namespace gorc {
namespace game {
namespace world {
namespace nothing {

class nothing_view : public view {
public:
	void update(double dt);
	void draw(double dt, const box<2, int>& view_size);
};

}
}
}
}
