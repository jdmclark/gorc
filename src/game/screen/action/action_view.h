#pragma once

#include "game/view.h"

namespace gorc {
namespace game {
namespace screen {
namespace action {

class action_presenter;

class action_view : public view {
	action_presenter* currentPresenter = nullptr;

public:
	inline void set_presenter(action_presenter* presenter) {
		currentPresenter = presenter;
	}

	void update(double dt);
	void draw(double dt, const box<2, int>& view_size);
};

}
}
}
}
