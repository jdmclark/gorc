#pragma once

#include "thingcontroller.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Gameplay {

class GhostController : public ThingController {
public:
	using ThingController::ThingController;

	virtual void Update(unsigned int thing_id, double dt) override;
	virtual unsigned int Create(const Content::Assets::Template& tpl, unsigned int sector_id,
					const Math::Vector<3>& pos, const Math::Vector<3>& orient) override;
};

}
}
}
}
}
