#pragma once

#include "music.h"
#include "framework/content/assets/sound.h"
#include "framework/utility/pool.h"
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
