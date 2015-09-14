#pragma once

#include "episode_entry_type.hpp"
#include "io/path.hpp"

namespace gorc {

    class episode_entry {
    public:
        int number;
        int disc;
        int level_number;
        episode_entry_type type;
        path filename;
        int light_power;
        int dark_power;
        int goto_a;
        int goto_b;

        episode_entry(int number,
                      int disc,
                      int level_number,
                      episode_entry_type type,
                      path const &filename,
                      int light_power,
                      int dark_power,
                      int goto_a,
                      int goto_b);
    };

}
