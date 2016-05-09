#pragma once

namespace gorc {
namespace game {
namespace world {
namespace inventory {

class player_bin {
public:
    bool activated = false;
    bool available = false;
    int value = 0;
    float cooldown = 0.0f;

    bool refiring = false;
    int mode = 0;
    float total_time_activated = 0.0f;
    float refire_time_activated = 0.0f;
    float refire_rate = 0.0f;
};

}
}
}
}
