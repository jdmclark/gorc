#include "sprite.hpp"

gorc::fourcc const gorc::content::assets::sprite::type = "SPR"_4CC;

gorc::content::assets::sprite::sprite(asset_ref<material> mat,
                                      int sprite_type,
                                      float width,
                                      float height,
                                      flags::geometry_mode geometry_mode,
                                      flags::light_mode light_mode,
                                      flags::texture_mode texture_mode,
                                      float extra_light,
                                      vector<3> const &offset)
    : mat(mat)
    , sprite_type(sprite_type)
    , width(width)
    , height(height)
    , geometry_mode(geometry_mode)
    , light_mode(light_mode)
    , texture_mode(texture_mode)
    , extra_light(extra_light)
    , offset(offset)
{
    return;
}
