#pragma once

#include "game/view.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Nothing {

class NothingView : public View {
public:
	void Update(double dt);
	void Draw(double dt, const Math::Box<2, unsigned int>& view_size);
};

}
}
}
}
