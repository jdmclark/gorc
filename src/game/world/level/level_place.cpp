#include "level_place.h"

gorc::game::world::level::level_place::level_place(std::shared_ptr<content::manager> contentmanager,
		const content::assets::level& level)
	: contentmanager(contentmanager), level(level) {
	return;
}

void gorc::game::world::level::level_place::accept(place_visitor& v) const {
	v.visit_level_place(*this);
}
