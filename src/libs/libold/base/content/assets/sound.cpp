#include "libold/base/content/assets/sound.hpp"
#include "content/content_manager.hpp"

gorc::fourcc const gorc::content::assets::sound::type = "WAV"_4CC;

gorc::asset_ref<gorc::content::assets::sound> gorc::get_asset(
        content_manager &cm,
        sound_id id)
{
    return cm.get_asset<content::assets::sound>(asset_id(static_cast<int>(id)));
}
