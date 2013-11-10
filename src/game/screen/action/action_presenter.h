#pragma once

#include "framework/place/presenter.h"
#include "game/components.h"
#include "action_place.h"

namespace gorc {
namespace game {
namespace screen {
namespace action {

class action_presenter : public gorc::place::presenter {
private:
	components& components;
	action_place place;

	bool window_has_focus = true;
	bool r_key_down = false;
	bool space_key_down = false;
	bool z_key_down = false;
	bool x_key_down = false;
	bool f1_key_down = false;
	bool g_key_down = false;

public:
	action_presenter(class components& components, const action_place& place);

	void start(event::event_bus& eventBus);
	void update(double dt);
};

}
}
}
}
