#include "material.hpp"
#include "content/content_manager.hpp"

gorc::fourcc const gorc::material::type = "MAT"_4CC;

gorc::asset_ref<gorc::material> gorc::get_asset(content_manager &cm, material_id id)
{
    return cm.get_asset<material>(asset_id(static_cast<int>(id)));
}
