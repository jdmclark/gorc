#include "components.h"

Gorc::Game::Components::Components(Diagnostics::Report& Report, Event::EventBus& EventBus, const sf::Input& Input, Content::VFS::VirtualFileSystem& FileSystem,
		Cog::Verbs::VerbTable& VerbTable, Cog::Compiler& Compiler,
		Place::PlaceController<Screen::Place>& ScreenPlaceController, Place::PlaceController<World::Place>& WorldPlaceController,
		ViewFrame& ScreenViewFrame, ViewFrame& WorldViewFrame,
		Screen::Action::ActionView& ActionView,
		World::Nothing::NothingView& NothingView, World::Level::LevelView& LevelView)
	: Report(Report), EventBus(EventBus), Input(Input), FileSystem(FileSystem),
	  VerbTable(VerbTable), Compiler(Compiler),
	  ScreenPlaceController(ScreenPlaceController), WorldPlaceController(WorldPlaceController),
	  ScreenViewFrame(ScreenViewFrame), WorldViewFrame(WorldViewFrame),
	  ActionView(ActionView),
	  NothingView(NothingView), LevelView(LevelView),
	  CurrentLevelPresenter(nullptr) {
	return;
}
