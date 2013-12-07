#pragma once

#include "cog/verbs/table.h"
#include "cog/compiler.h"
#include "game/world/level_presenter.h"
#include <memory>

namespace gorc {
namespace game {

class level_state {
public:
	cog::verbs::verb_table verb_table;
	std::unique_ptr<gorc::game::world::level_presenter> current_level_presenter;
	cog::compiler compiler;

	level_state();
};

}
}
