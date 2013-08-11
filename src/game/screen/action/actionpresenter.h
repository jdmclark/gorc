#pragma once

#include "framework/place/presenter.h"
#include "game/components.h"
#include "actionplace.h"

namespace Gorc {
namespace Game {
namespace Screen {
namespace Action {

class ActionPresenter : public Gorc::Place::Presenter {
private:
	Components& components;
	ActionPlace place;

	bool window_has_focus = true;
	bool r_key_down = false;
	bool space_key_down = false;
	bool z_key_down = false;
	bool x_key_down = false;
	bool f1_key_down = false;

public:
	ActionPresenter(Components& components, const ActionPlace& place);

	void Start(Event::EventBus& eventBus);
	void Update(double dt);
};

}
}
}
}
