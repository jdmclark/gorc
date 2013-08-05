#pragma once

#include "framework/math/vector.h"

namespace Gorc {

namespace Content {
namespace Assets {
class Template;
}
}

namespace Game {
namespace World {
namespace Level {

class LevelPresenter;

namespace Gameplay {

class ThingController {
protected:
	LevelPresenter& presenter;

public:
	ThingController(LevelPresenter& presenter);
	virtual ~ThingController();

	virtual void Update(unsigned int thing_id, double dt) = 0;
	virtual unsigned int Create(const Content::Assets::Template& tpl, unsigned int sector_id,
			const Math::Vector<3>& pos, const Math::Vector<3>& orient) = 0;
};

}
}
}
}
}
