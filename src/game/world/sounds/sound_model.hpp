#pragma once

#include "music.hpp"
#include "libold/base/content/assets/sound.hpp"
#include "libold/base/utility/pool.hpp"

namespace gorc {
namespace game {
namespace world {
namespace sounds {

class sound_model {
public:
    sf::Sound ambient_sound;
    music ambient_music;
};

}
}
}
}
