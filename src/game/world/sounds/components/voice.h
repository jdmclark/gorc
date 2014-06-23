#pragma once

#include "base/utility/entity_id.h"

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
