#include "level_state.hpp"
#include "game/world/level_presenter.hpp"
#include "jk/cog/vm/default_verbs.hpp"

#include "world/animations/components/slide_ceiling_sky.hpp"
#include "world/animations/components/slide_surface.hpp"
#include "world/animations/components/surface_animation.hpp"
#include "world/animations/components/surface_light.hpp"
#include "world/animations/components/surface_material.hpp"
#include "world/components/actor.hpp"
#include "world/components/character.hpp"
#include "world/components/class_sounds.hpp"
#include "world/components/item.hpp"
#include "world/components/player.hpp"
#include "world/components/pov_model.hpp"
#include "world/components/puppet_animations.hpp"
#include "world/components/thing.hpp"
#include "world/components/weapon.hpp"
#include "world/inventory/player_inventory.hpp"
#include "world/keys/components/key_mix.hpp"
#include "world/keys/components/key_state.hpp"
#include "world/keys/components/pov_key_mix.hpp"
#include "world/sounds/components/foley.hpp"
#include "world/sounds/components/sound.hpp"
#include "world/sounds/components/stop_when_destroyed.hpp"
#include "world/sounds/components/thing_sound.hpp"
#include "world/sounds/components/voice.hpp"

gorc::game::level_state::level_state(service_registry const &parent_services)
    : services(parent_services)
    , compiler(verbs, constants) {
    cog::default_populate_constant_table(constants);
    cog::default_populate_verb_table(verbs);

    services.add(components);
    services.add<gorc::cog::compiler>(compiler);
    services.add(colormap);
    services.add(verbs);

    components.register_component_type<world::animations::components::slide_ceiling_sky>();
    components.register_component_type<world::animations::components::slide_surface>();
    components.register_component_type<world::animations::components::surface_animation>();
    components.register_component_type<world::animations::components::surface_light>();
    components.register_component_type<world::animations::components::surface_material>();
    components.register_component_type<world::components::actor>();
    components.register_component_type<world::components::character>();
    components.register_component_type<world::components::class_sounds>();
    components.register_component_type<world::components::item>();
    components.register_component_type<world::components::player>();
    components.register_component_type<world::components::pov_model>();
    components.register_component_type<world::components::puppet_animations>();
    components.register_component_type<world::components::thing>();
    components.register_component_type<world::components::weapon>();
    components.register_component_type<world::inventory::player_inventory>();
    components.register_component_type<world::keys::key_mix>();
    components.register_component_type<world::keys::key_state>();
    components.register_component_type<world::keys::pov_key_mix>();
    components.register_component_type<world::sounds::components::foley>();
    components.register_component_type<world::sounds::components::sound>();
    components.register_component_type<world::sounds::components::stop_when_destroyed>();
    components.register_component_type<world::sounds::components::thing_sound>();
    components.register_component_type<world::sounds::components::voice>();
    return;
}
