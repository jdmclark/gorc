#pragma once

#include "framework/place/presenter.h"
#include "game/components.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Nothing {

class NothingPresenter : public Gorc::Place::Presenter {
private:
	Components& components;

public:
	NothingPresenter(Components& components);

	void Start(Event::EventBus& eventBus);
};

}
}
}
}
