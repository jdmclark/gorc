#pragma once

#include "charactercontroller.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Gameplay {

class ActorController : public CharacterController {
public:
	using CharacterController::CharacterController;

	virtual unsigned int Create(const Content::Assets::Template& tpl, unsigned int sector_id,
					const Math::Vector<3>& pos, const Math::Vector<3>& orient) override;
};

}
}
}
}
}
