#include "inventory_loader.hpp"
#include "libold/content/assets/sprite.hpp"
#include "content/content_manager.hpp"
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
        content_manager& manager, service_registry const &) const {

    auto mat = manager.load<assets::material>(t.get_space_delimited_string());
    auto sprite_type = t.get_number<int>();
    auto width = t.get_number<float>();
    auto height = t.get_number<float>();
    auto geometry_mode = static_cast<flags::geometry_mode>(t.get_number<uint32_t>());
    auto light_mode = static_cast<flags::light_mode>(t.get_number<uint32_t>());
    auto texture_mode = static_cast<flags::texture_mode>(t.get_number<uint32_t>());
    auto extra_light = t.get_number<float>();
    auto offset_x = t.get_number<float>();
    auto offset_y = t.get_number<float>();
    auto offset_z = t.get_number<float>();

    return std::make_unique<content::assets::sprite>(mat,
                                                     sprite_type,
                                                     width,
                                                     height,
                                                     geometry_mode,
                                                     light_mode,
                                                     texture_mode,
                                                     extra_light,
                                                     make_vector(offset_x,
                                                                 offset_y,
                                                                 offset_z));
}
