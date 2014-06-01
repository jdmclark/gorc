#pragma once

#include "base/utility/entity_id.h"
#include "content/flags/armed_mode.h"

namespace gorc {
namespace game {
namespace world {
namespace events {

class armed_mode_changed {
public:
    #include "armed_mode_changed.uid"

    entity_id thing;
    flags::armed_mode mode;

    armed_mode_changed(entity_id thing, flags::armed_mode mode);
};

}
}
}
}
