#include "inventory_loader.h"
#include "content/assets/sprite.h"
#include "framework/diagnostics/helper.h"
#include "framework/io/exception.h"
#include "framework/content/manager.h"
#include <boost/format.hpp>

const std::vector<boost::filesystem::path> gorc::content::loaders::sprite_loader::asset_root_path = { "misc/spr" };

gorc::content::loaders::sprite_loader::sprite_loader(const content::assets::colormap& colormap) : colormap(colormap) {
    return;
}

std::unique_ptr<gorc::content::asset> gorc::content::loaders::sprite_loader::parse(text::tokenizer& t,
        content::manager& manager, diagnostics::report& report) {
    std::unique_ptr<content::assets::sprite> spr(new content::assets::sprite());

    spr->mat = &manager.load<assets::material>(t.get_space_delimited_string(), colormap);
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
