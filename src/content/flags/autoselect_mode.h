#pragma once

namespace gorc {
namespace flags {

enum class autoselect_mode {
    player_mounting = -1,
    prev_next_query = 0,
    cog_query = 0,
    cog_best_weapon_query = 1,
    powerup_cog_best_weapon_query = 2
};

}
}
