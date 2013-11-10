#include "components.h"

gorc::game::components::components(diagnostics::report& report, event::event_bus& EventBus, sf::Window& Window,
		content::vfs::virtual_filesystem& FileSystem, cog::verbs::verb_table& verb_table, cog::compiler& compiler,
		place::place_controller<screen::place>& ScreenPlaceController, place::place_controller<world::place>& WorldPlaceController,
		view_frame& ScreenViewFrame, view_frame& WorldViewFrame,
		screen::action::action_view& ActionView,
		world::nothing::nothing_view& nothing_view, world::level::level_view& level_view)
	: report(report), event_bus(EventBus), window(Window), filesystem(FileSystem),
	  verb_table(verb_table), compiler(compiler),
	  screen_place_controller(ScreenPlaceController), world_place_controller(WorldPlaceController),
	  screen_view_frame(ScreenViewFrame), world_view_frame(WorldViewFrame),
	  action_view(ActionView),
	  nothing_view(nothing_view), level_view(level_view),
	  current_level_presenter(nullptr) {
	return;
}
