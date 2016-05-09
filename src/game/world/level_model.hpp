#pragma once

#include "math/vector.hpp"
#include "math/color.hpp"
#include "libold/content/assets/level.hpp"
#include "ecs/entity_component_system.hpp"
#include "components/thing.hpp"
#include "surface.hpp"
#include "jk/cog/vm/executor.hpp"
#include "game/world/sounds/sound_model.hpp"
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

    entity_component_system<thing_id> ecs;
    service_registry services;

    cog::executor script_model;
    sounds::sound_model sound_model;
    camera::camera_model camera_model;
    world_value_mapping value_mapping;

    thing_id local_player_thing_id;

    std::vector<content::assets::thing_template const*> spawn_points;
    size_t current_spawn_point = 0UL;

    double level_time = 0.0;
    double game_time = 0.0;
    color_rgb dynamic_tint = make_color(0.0f, 0.0f, 0.0f);

    level_model(event_bus& parent_event_bus, content_manager& manager, service_registry const &,
            asset_ref<content::assets::level> level);

    components::thing& get_thing(thing_id id);

    cog::source_type get_thing_source_type(cog::value id);

    // Level specific send_to_linked wrapper.
    // Automatically includes correct source type and calls thing class/capture cogs.
    void send_to_linked(cog::message_type msg,
                        cog::value sender,
                        cog::value source,
                        cog::value param0 = cog::value(),
                        cog::value param1 = cog::value(),
                        cog::value param2 = cog::value(),
                        cog::value param3 = cog::value());
};

}
}
}
