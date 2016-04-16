#include "model.hpp"
#include "content/content_manager.hpp"

gorc::fourcc const gorc::content::assets::model::type = "3DO"_4CC;

gorc::asset_ref<gorc::content::assets::model> gorc::get_asset(
        content_manager &cm,
        model_id id)
{
    return cm.get_asset<content::assets::model>(asset_id(static_cast<int>(id)));
}
