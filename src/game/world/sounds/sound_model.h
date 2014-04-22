#pragma once

#include "music.h"
#include "base/content/assets/sound.h"
#include "base/utility/pool.h"
#include "sound.h"

namespace gorc {
namespace game {
namespace world {
namespace sounds {

class sound_model {
public:
    sf::Sound ambient_sound;
    music ambient_music;
    pool<sound, 8> sounds;
};

}
}
}
}
