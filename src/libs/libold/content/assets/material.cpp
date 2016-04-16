#include "material.hpp"
#include "content/content_manager.hpp"

gorc::fourcc const gorc::content::assets::material::type = "MAT"_4CC;

gorc::asset_ref<gorc::content::assets::material> gorc::get_asset(
        content_manager &cm,
        material_id id)
{
    return cm.get_asset<content::assets::material>(asset_id(static_cast<int>(id)));
}
