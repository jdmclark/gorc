#pragma once

#include "game/view.h"

namespace Gorc {
namespace Game {
namespace Screen {
namespace Action {

class ActionPresenter;

class ActionView : public View {
	ActionPresenter* currentPresenter = nullptr;

public:
	inline void SetPresenter(ActionPresenter* presenter) {
		currentPresenter = presenter;
	}

	void Update(double dt);
	void Draw(double dt, const Math::Box<2, unsigned int>& view_size);
};

}
}
}
}
