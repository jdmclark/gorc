#include "register_legacy_loaders.hpp"

#include "libold/base/content/loaders/shader_loader.hpp"
#include "libold/base/content/loaders/shader_program_loader.hpp"
#include "libold/base/content/loaders/sound_loader.hpp"

#include "libold/content/loaders/animation_loader.hpp"
#include "libold/content/loaders/inventory_loader.hpp"
#include "libold/content/loaders/level_loader.hpp"
#include "libold/content/loaders/material_loader.hpp"
#include "libold/content/loaders/model_loader.hpp"
#include "libold/content/loaders/puppet_loader.hpp"
#include "libold/content/loaders/soundclass_loader.hpp"
#include "libold/content/loaders/sprite_loader.hpp"

#include "jk/cog/compiler/script_loader.hpp"
#include "jk/content/colormap_loader.hpp"

void gorc::content::register_legacy_loaders(loader_registry &loaders)
{
    loaders.emplace_loader<loaders::shader_loader>();
    loaders.emplace_loader<loaders::vertex_program_loader>();
    loaders.emplace_loader<loaders::fragment_program_loader>();
    loaders.emplace_loader<loaders::sound_loader>();

    loaders.emplace_loader<loaders::animation_loader>();
    loaders.emplace_loader<loaders::inventory_loader>();
    loaders.emplace_loader<loaders::level_loader>();
    loaders.emplace_loader<loaders::material_loader>();
    loaders.emplace_loader<loaders::model_loader>();
    loaders.emplace_loader<loaders::puppet_loader>();
    loaders.emplace_loader<loaders::soundclass_loader>();
    loaders.emplace_loader<loaders::sprite_loader>();

    loaders.emplace_loader<cog::script_loader>();
    loaders.emplace_loader<colormap_loader>();
}
