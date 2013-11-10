#pragma once

#include "game/world/place.h"
#include "content/assets/level.h"
#include "content/manager.h"
#include <memory>

namespace gorc {
namespace game {
namespace world {
namespace level {

class level_place : public place {
public:
	std::shared_ptr<content::manager> contentmanager;
	const content::assets::level& level;

	level_place(std::shared_ptr<content::manager> contentmanager, const content::assets::level& level);

	void accept(place_visitor& v) const;
};

}
}
}
}
