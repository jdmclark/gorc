#include "components.h"

#include "framework/events/printevent.h"
#include "framework/events/exitingevent.h"
#include "framework/events/exitevent.h"
#include "framework/events/shutdownevent.h"
#include "framework/place/placecontroller.h"

#include "game/screen/presentermapper.h"
#include "game/world/presentermapper.h"

#include "game/screen/action/actionplace.h"
#include "game/world/level/levelplace.h"

#include <iostream>
#include <cmath>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <boost/algorithm/string/predicate.hpp>

#include "framework/diagnostics/streamreport.h"
#include "content/manager.h"
#include "content/assets/level.h"

const double gameplayTick = (1.0 / 60.0);

int main(int argc, char** argv) {
	sf::Window Window(sf::VideoMode(1280, 720, 32), "Gorc");
	const sf::Input& Input = Window.GetInput();

	Gorc::Diagnostics::StreamReport Report(std::cout);
	Gorc::Event::EventBus EventBus;
	Gorc::Content::VFS::VirtualFileSystem FileSystem("game/restricted",
			"game/resource", "game/episode", Report);

	Gorc::Game::Screen::PresenterMapper ScreenPresenterMapper;
	Gorc::Place::PlaceController<Gorc::Game::Screen::Place> ScreenPlaceController(EventBus, ScreenPresenterMapper);

	Gorc::Game::World::PresenterMapper WorldPresenterMapper;
	Gorc::Place::PlaceController<Gorc::Game::World::Place> WorldPlaceController(EventBus, WorldPresenterMapper);

	Gorc::Game::ViewFrame ScreenViewFrame(Window);
	Gorc::Game::ViewFrame WorldViewFrame(Window);

	Gorc::Game::Screen::Action::ActionView ActionView;

	Gorc::Game::World::Nothing::NothingView NothingView;
	Gorc::Game::World::Level::LevelView LevelView;

	Gorc::Game::Components Components(Report, EventBus, Input, FileSystem,
			ScreenPlaceController, WorldPlaceController, ScreenViewFrame, WorldViewFrame,
			ActionView, NothingView, LevelView);

	ScreenPresenterMapper.SetComponents(Components);
	WorldPresenterMapper.SetComponents(Components);

	bool running = true;

	// Register core event handlers
	EventBus.AddHandler<Gorc::Events::PrintEvent>([](Gorc::Events::PrintEvent& e) {
		std::cout << e.message << std::endl;
	});

	EventBus.AddHandler<Gorc::Events::ExitEvent>([&EventBus, &running](Gorc::Events::ExitEvent& e) {
		// A component has requested an application exit.
		// Check if components can quit.
		Gorc::Events::ExitingEvent exitingEvent;
		EventBus.FireEvent(exitingEvent);

		if(exitingEvent.Cancel) {
			return;
		}

		// All components can quit. Request shutdown.
		Gorc::Events::ShutdownEvent shutdownEvent;
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
	auto contentManager = std::make_shared<Gorc::Content::Manager>(Report, FileSystem);
	const auto& lev = contentManager->Load<Gorc::Content::Assets::Level>("01narshadda.jkl");
	WorldPlaceController.GoTo(Gorc::Game::World::Level::LevelPlace(contentManager, lev));

	ScreenPlaceController.GoTo(Gorc::Game::Screen::Action::ActionPlace());
	// END HACK

	// Game loop:
	sf::Clock clock;
	double gameplayAccumulator = 0.0;
	double gameplayElapsedTime = 0.0;

	sf::Event event;
	while(running) {
		while(Window.GetEvent(event)) {
			if(event.Type == sf::Event::Closed) {
				Gorc::Events::ExitEvent exitEvent;
				EventBus.FireEvent(exitEvent);
			}
			else if(event.Type == sf::Event::Resized) {
				glViewport(0, 0, event.Size.Width, event.Size.Height);
			}
		}

		Window.SetActive();

		double currentTime = clock.GetElapsedTime();
		clock.Reset();

		gameplayElapsedTime += currentTime;
		gameplayAccumulator += currentTime;
		if(gameplayAccumulator >= gameplayTick) {
			gameplayAccumulator -= gameplayTick;

			// Update simulation.
			WorldViewFrame.Update(gameplayTick);
			ScreenViewFrame.Update(gameplayTick);
		}

		// TODO: Render here
		glClearDepth(1.0f);
		glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		WorldViewFrame.Draw(currentTime);
		ScreenViewFrame.Draw(currentTime);

		Window.Display();
	}

	return 0;
}
