#pragma once

#include <SFML/Audio.hpp>
#include "base/content/asset.h"
#include "base/content/loaders/sound_loader.h"

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
