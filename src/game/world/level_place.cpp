#include "level_place.hpp"

gorc::game::world::level_place::level_place(std::shared_ptr<content::content_manager> contentmanager,
                                            asset_ref<content::assets::level> level)
    : contentmanager(contentmanager), level(level) {
    return;
}
