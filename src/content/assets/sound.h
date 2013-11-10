#pragma once

#include <SFML/Audio.hpp>
#include "content/asset.h"
#include "content/loaders/sound_loader.h"

namespace gorc {
namespace content {
namespace assets {

class sound : public asset {
public:
	using loader = loaders::sound_loader;

	sf::SoundBuffer buffer;
};

}
}
}
