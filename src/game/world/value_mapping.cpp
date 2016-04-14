#include "value_mapping.hpp"
#include "log/log.hpp"
#include "libold/content/assets/colormap.hpp"
#include "libold/content/assets/animation.hpp"
#include "libold/content/assets/material.hpp"
#include "libold/content/assets/model.hpp"
#include "libold/base/content/assets/sound.hpp"

gorc::game::world::world_value_mapping::world_value_mapping(
        content_manager &content,
        cog::executor &exec,
        asset_ref<content::assets::level> level)
    : content(content)
    , exec(exec)
    , level(level)
{
    return;
}

namespace {
    template <typename OtherIdT>
    OtherIdT force_cast(gorc::asset_id id)
    {
        return OtherIdT(static_cast<int>(id));
    }
}

gorc::cog::value gorc::game::world::world_value_mapping::operator()(
        cog::value_type type,
        std::string const &value) const
{
    switch(type) {
    default:
        LOG_FATAL(format("world value mapping for type %s is not implemented") %
                  as_string(type));

    case cog::value_type::cog:
        return exec.create_global_instance(content.load<cog::script>(value));

    case cog::value_type::colormap:
        return force_cast<colormap_id>(content.load_id<content::assets::colormap>(value));

    case cog::value_type::keyframe:
        return force_cast<keyframe_id>(content.load_id<content::assets::animation>(value));

    case cog::value_type::material:
        return force_cast<material_id>(content.load_id<content::assets::material>(value));

    case cog::value_type::model:
        return force_cast<model_id>(content.load_id<content::assets::model>(value));

    case cog::value_type::sound:
        return force_cast<sound_id>(content.load_id<content::assets::sound>(value));

    case cog::value_type::thing_template: {
            auto it = level->template_map.find(value);
            if(it == level->template_map.end()) {
                LOG_FATAL(format("referenced undefined template %s") % value);
            }

            return thing_template_id(it->second);
        }
        break;
    }
}
