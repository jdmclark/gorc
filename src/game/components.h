#pragma once

#include <GL/glew.h>

#include "framework/diagnostics/report.h"
#include "framework/event/event_bus.h"
#include "framework/place/place_controller.h"
#include "framework/utility/randomizer.h"

#include "content/vfs/virtual_filesystem.h"

#include "cog/verbs/table.h"
#include "cog/compiler.h"

#include "game/view_frame.h"

#include "game/screen/place.h"
#include "game/screen/action/action_view.h"

#include "game/world/place.h"
#include "game/world/nothing/nothing_view.h"
#include "game/world/level/level_view.h"
#include "game/world/level/level_presenter.h"

#include <SFML/Window.hpp>
#include <memory>

namespace gorc {
namespace game {

class components {
public:
	diagnostics::report& report;
	event::event_bus& event_bus;
	sf::Window& window;
	randomizer randomizer;

	content::vfs::virtual_filesystem& filesystem;

	cog::verbs::verb_table& verb_table;
	cog::compiler& compiler;

	place::place_controller<screen::place>& screen_place_controller;
	place::place_controller<world::place>& world_place_controller;

	view_frame& screen_view_frame;
	view_frame& world_view_frame;

	screen::action::action_view& action_view;

	world::nothing::nothing_view& nothing_view;
	world::level::level_view& level_view;

	world::level::level_presenter* current_level_presenter = nullptr;

	components(diagnostics::report& report, event::event_bus& EventBus, sf::Window& Window,
			content::vfs::virtual_filesystem& FileSystem, cog::verbs::verb_table& verb_table, cog::compiler& compiler,
			place::place_controller<screen::place>& ScreenPlaceController, place::place_controller<world::place>& WorldPlaceController,
			view_frame& ScreenViewFrame, view_frame& WorldViewFrame,
			screen::action::action_view& ActionView,
			world::nothing::nothing_view& nothing_view, world::level::level_view& level_view);
};

}
}
