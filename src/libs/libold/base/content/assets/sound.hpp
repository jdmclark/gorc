#pragma once

#include <SFML/Audio.hpp>
#include "content/asset.hpp"
#include "libold/base/content/loaders/sound_loader.hpp"

namespace gorc {

class content_manager;

namespace content {
namespace assets {

class sound : public asset {
public:
    static fourcc const type;

    sf::SoundBuffer buffer;
};

}
}

asset_ref<content::assets::sound> get_asset(content_manager &cm, sound_id);

}
