#include "inventory_loader.hpp"
#include "libold/content/assets/sprite.hpp"
#include "libold/base/content/content_manager.hpp"
#include <boost/format.hpp>

gorc::fourcc const gorc::content::loaders::sprite_loader::type = "SPR"_4CC;

namespace {
    const std::vector<gorc::path> asset_root_path = { "misc/spr" };
}

std::vector<gorc::path> const& gorc::content::loaders::sprite_loader::get_prefixes() const
{
    return asset_root_path;
}

std::unique_ptr<gorc::asset> gorc::content::loaders::sprite_loader::parse(text::tokenizer& t,
        content::content_manager& manager, service_registry const &) const {
    std::unique_ptr<content::assets::sprite> spr(new content::assets::sprite());

    spr->mat = &manager.load<assets::material>(t.get_space_delimited_string());
    spr->type = t.get_number<int>();
    spr->width = t.get_number<float>();
    spr->height = t.get_number<float>();
    spr->geometry_mode = static_cast<flags::geometry_mode>(t.get_number<uint32_t>());
    spr->light_mode = static_cast<flags::light_mode>(t.get_number<uint32_t>());
    spr->texture_mode = static_cast<flags::texture_mode>(t.get_number<uint32_t>());
    spr->extra_light = t.get_number<float>();
    get<0>(spr->offset) = t.get_number<float>();
    get<1>(spr->offset) = t.get_number<float>();
    get<2>(spr->offset) = t.get_number<float>();

    return std::unique_ptr<asset>(std::move(spr));
}
