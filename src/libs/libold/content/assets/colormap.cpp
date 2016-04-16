#include "colormap.hpp"
#include "content/content_manager.hpp"

gorc::fourcc const gorc::content::assets::colormap::type = "CMP"_4CC;

gorc::asset_ref<gorc::content::assets::colormap> gorc::get_asset(
        content_manager &cm,
        colormap_id id)
{
    return cm.get_asset<content::assets::colormap>(asset_id(static_cast<int>(id)));
}
