#pragma once

#include "framework/diagnostics/report.h"
#include "framework/event/eventbus.h"
#include "framework/place/placecontroller.h"

#include "content/vfs/virtualfilesystem.h"

#include "game/viewframe.h"

#include "game/screen/place.h"
#include "game/screen/action/actionview.h"

#include "game/world/place.h"
#include "game/world/nothing/nothingview.h"
#include "game/world/level/levelview.h"

#include <SFML/Window.hpp>
#include <memory>

namespace Gorc {
namespace Game {

class Components {
public:
	Diagnostics::Report& Report;
	Event::EventBus& EventBus;
	const sf::Input& Input;

	Content::VFS::VirtualFileSystem& FileSystem;

	Place::PlaceController<Screen::Place>& ScreenPlaceController;
	Place::PlaceController<World::Place>& WorldPlaceController;

	ViewFrame& ScreenViewFrame;
	ViewFrame& WorldViewFrame;

	Screen::Action::ActionView& ActionView;

	World::Nothing::NothingView& NothingView;
	World::Level::LevelView& LevelView;

	Components(Diagnostics::Report& Report, Event::EventBus& EventBus, const sf::Input& Input, Content::VFS::VirtualFileSystem& FileSystem,
			Place::PlaceController<Screen::Place>& ScreenPlaceController, Place::PlaceController<World::Place>& WorldPlaceController,
			ViewFrame& ScreenViewFrame, ViewFrame& WorldViewFrame,
			Screen::Action::ActionView& ActionView,
			World::Nothing::NothingView& NothingView, World::Level::LevelView& LevelView);
};

}
}
