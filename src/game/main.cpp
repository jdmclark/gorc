#include "constants.h"
#include "content/assets/shader.h"
#include "components.h"

#include "framework/events/print_event.h"
#include "framework/events/exiting_event.h"
#include "framework/events/exit_event.h"
#include "framework/events/shutdown_event.h"
#include "game/events/window_focus_event.h"
#include "framework/place/place_controller.h"

#include "game/screen/presenter_mapper.h"
#include "game/world/presenter_mapper.h"

#include "game/screen/action/action_place.h"
#include "game/world/level/level_place.h"

#include <iostream>
#include <cmath>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <boost/algorithm/string/predicate.hpp>

#include "framework/diagnostics/stream_report.h"
#include "content/manager.h"
#include "content/assets/level.h"
#include "cog/compiler.h"

#include <iostream>

using namespace Gorc;

int main(int argc, char** argv) {
	// Create window and OpenGL context.
	sf::Window Window(sf::VideoMode(1280, 720, 32), "Gorc");
	Window.setVerticalSyncEnabled(true);
	Window.setActive();

	// Initialize GLEW.
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		// TODO: Print error.
		return 1;
	}

	Diagnostics::StreamReport Report(std::cout);
	Event::EventBus EventBus;
	Content::VFS::VirtualFileSystem FileSystem("game/restricted",
			"game/resource", "game/episode", Report);

	auto systemContentManager = std::make_shared<Content::Manager>(Report, FileSystem);

	Cog::Verbs::VerbTable VerbTable;
	Cog::Compiler Compiler(VerbTable);

	Game::Screen::PresenterMapper ScreenPresenterMapper;
	Place::PlaceController<Game::Screen::Place> ScreenPlaceController(EventBus, ScreenPresenterMapper);

	Game::World::PresenterMapper WorldPresenterMapper;
	Place::PlaceController<Game::World::Place> WorldPlaceController(EventBus, WorldPresenterMapper);

	Game::ViewFrame ScreenViewFrame(Window);
	Game::ViewFrame WorldViewFrame(Window);

	Game::Screen::Action::ActionView ActionView;

	Game::World::Nothing::NothingView NothingView;
	Game::World::Level::LevelView LevelView(*systemContentManager);

	Game::Components Components(Report, EventBus, Window, FileSystem, VerbTable, Compiler,
			ScreenPlaceController, WorldPlaceController, ScreenViewFrame, WorldViewFrame,
			ActionView, NothingView, LevelView);

	ScreenPresenterMapper.SetComponents(Components);
	WorldPresenterMapper.SetComponents(Components);

	// Register verbs
	Game::World::Level::LevelPresenter::RegisterVerbs(VerbTable, Components);

	bool running = true;

	// Register core event handlers
	EventBus.AddHandler<Events::PrintEvent>([](Events::PrintEvent& e) {
		std::cout << e.message << std::endl;
	});

	EventBus.AddHandler<Events::ExitEvent>([&EventBus, &running](Events::ExitEvent& e) {
		// A component has requested an application exit.
		// Check if components can quit.
		Events::ExitingEvent exitingEvent;
		EventBus.FireEvent(exitingEvent);

		if(exitingEvent.Cancel) {
			return;
		}

		// All components can quit. Request shutdown.
		Events::ShutdownEvent shutdownEvent;
		EventBus.FireEvent(shutdownEvent);

		running = false;
	});

	// HACK: Set current episode to The Force Within.
	for(size_t i = 0; i < FileSystem.GetEpisodeCount(); ++i) {
		if(boost::iequals(FileSystem.GetEpisode(i).GetEpisodeName(), "The Force Within")) {
			FileSystem.SetEpisode(i);
		}
	}

	// HACK: Set current level to 01narshadda.jkl.
	auto contentManager = std::make_shared<Content::Manager>(Report, FileSystem);
	const auto& lev = contentManager->Load<Content::Assets::Level>("01narshadda.jkl", Compiler);
	WorldPlaceController.GoTo(Game::World::Level::LevelPlace(contentManager, lev));

	ScreenPlaceController.GoTo(Game::Screen::Action::ActionPlace());
	// END HACK

	// Game loop:
	sf::Clock clock;
	double gameplayAccumulator = 0.0;
	double gameplayElapsedTime = 0.0;

	sf::Event event;
	while(running) {
		while(Window.pollEvent(event)) {
			switch(event.type) {
			case sf::Event::Closed: {
				Events::ExitEvent exitEvent;
				EventBus.FireEvent(exitEvent);
			}
			break;

			case sf::Event::Resized:
				glViewport(0, 0, event.size.width, event.size.height);
				break;

			case sf::Event::LostFocus: {
				Game::Events::WindowFocusEvent focusEvent(false);
				EventBus.FireEvent(focusEvent);
			}
			break;

			case sf::Event::GainedFocus: {
				Game::Events::WindowFocusEvent focusEvent(true);
				EventBus.FireEvent(focusEvent);
			}
			break;

			default:
				break;
			}
		}

		Window.setActive();

		double currentTime = clock.getElapsedTime().asSeconds();
		clock.restart();

		gameplayElapsedTime += currentTime;
		gameplayAccumulator += currentTime;

		while(gameplayAccumulator >= GameplayTick) {
			gameplayAccumulator -= GameplayTick;

			// Update simulation.
			WorldViewFrame.Update(GameplayTick);
			ScreenViewFrame.Update(GameplayTick);
		}

		// TODO: Render here
		glClearDepth(1.0f);
		glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		WorldViewFrame.Draw(currentTime);
		ScreenViewFrame.Draw(currentTime);

		Window.display();
	}

	return 0;
}
