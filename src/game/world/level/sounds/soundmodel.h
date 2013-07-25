#pragma once

#include "music.h"
#include "sound.h"
#include "framework/pool.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Sounds {

class SoundModel {
public:
	sf::Sound AmbientSound;
	Music AmbientMusic;
	Pool<Sound, 8> Sounds;
};

}
}
}
}
}
