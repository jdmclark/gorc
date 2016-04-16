#include "animation.hpp"
#include "content/content_manager.hpp"

gorc::fourcc const gorc::content::assets::animation::type = "KEY"_4CC;

gorc::asset_ref<gorc::content::assets::animation> gorc::get_asset(
        content_manager &cm,
        keyframe_id id)
{
    return cm.get_asset<content::assets::animation>(asset_id(static_cast<int>(id)));
}
