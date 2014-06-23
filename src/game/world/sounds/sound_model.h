#pragma once

#include "music.h"
#include "base/content/assets/sound.h"
#include "base/utility/pool.h"

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
