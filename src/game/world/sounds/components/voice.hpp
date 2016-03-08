#pragma once

#include "libold/base/utility/entity_id.hpp"

namespace gorc {
namespace game {
namespace world {
namespace sounds {
namespace components {

class voice {
public:
    #include "voice.uid"

    entity_id sound;

    voice(entity_id sound);
};

}
}
}
}
}
