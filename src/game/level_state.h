#pragma once

#include "cog/verbs/table.h"
#include "cog/compiler.h"
#include <memory>

namespace gorc {
namespace game {

namespace world {
class level_presenter;
}

class level_state {
public:
    cog::verbs::verb_table verb_table;
    std::unique_ptr<gorc::game::world::level_presenter> current_level_presenter;
    cog::compiler compiler;

    level_state();
};

}
}
