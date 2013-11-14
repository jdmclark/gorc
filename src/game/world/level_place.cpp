#include "level_place.h"

gorc::game::world::level_place::level_place(std::shared_ptr<content::manager> contentmanager,
		const content::assets::level& level)
	: contentmanager(contentmanager), level(level) {
	return;
}
