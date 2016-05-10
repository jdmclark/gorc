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

template <>
struct id_asset_type<sound_id> {
    using type = content::assets::sound;
};

}
