#pragma once

#include "music.h"
#include "sound.h"
#include "framework/pool.h"

namespace gorc {
namespace game {
namespace world {
namespace level {
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
}
