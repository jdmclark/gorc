#pragma once

#include <SFML/Audio.hpp>
#include "content/asset.h"
#include "content/loaders/sound_loader.h"

namespace Gorc {
namespace Content {
namespace Assets {

class Sound : public Asset {
public:
	using Loader = Loaders::SoundLoader;

	sf::SoundBuffer Buffer;
};

}
}
}
