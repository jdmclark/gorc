#pragma once

#include "base/math/vector.h"
#include "content/assets/level.h"
#include "cog/instance.h"
#include "base/utility/pool.h"
#include "thing.h"
#include "surface.h"
#include "sounds/sound.h"
#include "game/world/animations/animation_model.h"
#include "game/world/scripts/script_model.h"
#include "game/world/sounds/sound_model.h"
#include "game/world/keys/key_model.h"
#include "game/world/inventory/inventory_model.h"
#include "game/world/camera/camera_model.h"
#include <vector>

namespace gorc {
namespace game {
namespace world {

class level_model {
public:
    const content::assets::level& level;
    content::assets::level_header header;
    std::vector<content::assets::level_adjoin> adjoins;
    std::vector<surface> surfaces;
    std::vector<content::assets::level_sector> sectors;

    animations::animation_model animation_model;
    scripts::script_model script_model;
    sounds::sound_model sound_model;
    keys::key_model key_model;
    inventory::inventory_model inventory_model;
    camera::camera_model camera_model;

    pool<thing> things;

    int local_player_thing_id;

    std::vector<content::assets::thing_template const*> spawn_points;
    size_t current_spawn_point = 0UL;

    double level_time = 0.0;
    double game_time = 0.0;
    vector<3> dynamic_tint = make_vector(0.0f, 0.0f, 0.0f);

    level_model(content::content_manager& manager, cog::compiler& Cogcompiler, const content::assets::level& level, const content::assets::inventory& inv);
};

}
}
}
