#pragma once

#include <GL/glew.h>

#include "framework/diagnostics/report.h"
#include "framework/event/event_bus.h"
#include "framework/place/place_controller.h"
#include "framework/randomizer.h"

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

namespace Gorc {
namespace Game {

class Components {
public:
	Diagnostics::Report& Report;
	Event::EventBus& EventBus;
	sf::Window& Window;
	Randomizer Randomizer;

	Content::VFS::VirtualFileSystem& FileSystem;

	Cog::Verbs::VerbTable& VerbTable;
	Cog::Compiler& Compiler;

	Place::PlaceController<Screen::Place>& ScreenPlaceController;
	Place::PlaceController<World::Place>& WorldPlaceController;

	ViewFrame& ScreenViewFrame;
	ViewFrame& WorldViewFrame;

	Screen::Action::ActionView& ActionView;

	World::Nothing::NothingView& NothingView;
	World::Level::LevelView& LevelView;

	World::Level::LevelPresenter* CurrentLevelPresenter = nullptr;

	Components(Diagnostics::Report& Report, Event::EventBus& EventBus, sf::Window& Window,
			Content::VFS::VirtualFileSystem& FileSystem, Cog::Verbs::VerbTable& VerbTable, Cog::Compiler& Compiler,
			Place::PlaceController<Screen::Place>& ScreenPlaceController, Place::PlaceController<World::Place>& WorldPlaceController,
			ViewFrame& ScreenViewFrame, ViewFrame& WorldViewFrame,
			Screen::Action::ActionView& ActionView,
			World::Nothing::NothingView& NothingView, World::Level::LevelView& LevelView);
};

}
}
