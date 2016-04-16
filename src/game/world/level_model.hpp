#pragma once

#include "math/vector.hpp"
#include "libold/content/assets/level.hpp"
#include "libold/base/utility/pool.hpp"
#include "libold/base/utility/component_system.hpp"
#include "components/thing.hpp"
#include "surface.hpp"
#include "jk/cog/vm/executor.hpp"
#include "game/world/sounds/sound_model.hpp"
#include "game/world/keys/key_model.hpp"
#include "game/world/inventory/inventory_model.hpp"
#include "game/world/camera/camera_model.hpp"
#include "value_mapping.hpp"
#include <vector>

namespace gorc {
namespace game {
namespace world {

class level_model {
public:
    asset_ref<content::assets::level> level;
    content::assets::level_header header;
    std::vector<content::assets::level_adjoin> adjoins;
    std::vector<surface> surfaces;
    std::vector<content::assets::level_sector> sectors;

    component_system ecs;
    service_registry services;

    cog::executor script_model;
    sounds::sound_model sound_model;
    keys::key_model key_model;
    inventory::inventory_model inventory_model;
    camera::camera_model camera_model;
    world_value_mapping value_mapping;

    thing_id local_player_thing_id;

    std::vector<content::assets::thing_template const*> spawn_points;
    size_t current_spawn_point = 0UL;

    double level_time = 0.0;
    double game_time = 0.0;
    vector<3> dynamic_tint = make_vector(0.0f, 0.0f, 0.0f);

    level_model(event_bus& parent_event_bus, content_manager& manager, service_registry const &,
            asset_ref<content::assets::level> level, asset_ref<content::assets::inventory> inv);

    components::thing& get_thing(thing_id id);

    cog::source_type get_thing_source_type(thing_id id);
};

}
}
}
