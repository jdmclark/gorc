#pragma once

#include "game/world/place.h"
#include "content/assets/level.h"
#include "content/manager.h"
#include <memory>

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class LevelPlace : public Place {
public:
	std::shared_ptr<Content::Manager> ContentManager;
	const Content::Assets::Level& Level;

	LevelPlace(std::shared_ptr<Content::Manager> ContentManager, const Content::Assets::Level& Level);

	void Accept(PlaceVisitor& v) const;
};

}
}
}
}
