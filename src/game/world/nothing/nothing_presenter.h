#pragma once

#include "framework/place/presenter.h"
#include "game/components.h"

namespace gorc {
namespace game {
namespace world {
namespace nothing {

class nothing_presenter : public gorc::place::presenter {
private:
	components& components;

public:
	nothing_presenter(class components& components);

	void start(event::event_bus& eventBus);
};

}
}
}
}
