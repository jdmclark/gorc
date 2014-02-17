#pragma once

#include <SFML/Audio.hpp>
#include "framework/content/asset.h"
#include "framework/content/loaders/sound_loader.h"

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
