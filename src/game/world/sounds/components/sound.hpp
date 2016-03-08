#pragma once

#include "libold/base/content/assets/sound.hpp"
#include "math/vector.hpp"
#include "libold/base/utility/easing.hpp"
#include <SFML/Audio.hpp>

namespace gorc {
namespace game {
namespace world {
namespace sounds {
namespace components {

class sound {
public:
    #include "sound.uid"

    sf::Sound internal_sound;
    bool has_played = false;

    bool do_distance_attenuation = false;
    float minimum_attenuation_radius = 0.0f;
    float maximum_attenuation_radius = 1.0f;
    vector<3> position;

    linear_eased<float> pitch, volume;
    float stop_delay = 0.0f;

    sound();
};

}
}
}
}
}
