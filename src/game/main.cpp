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

using namespace gorc;

int main(int argc, char** argv) {
	// create window and OpenGL context.
	sf::Window Window(sf::VideoMode(1280, 720, 32), "Gorc");
	Window.setVerticalSyncEnabled(true);
	Window.setActive();

	// Initialize GLEW.
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		// TODO: Print error.
		return 1;
	}

	diagnostics::stream_report report(std::cout);
	event::event_bus event_bus;
	content::vfs::virtual_filesystem FileSystem("game/restricted",
			"game/resource", "game/episode", report);

	auto systemcontentmanager = std::make_shared<content::manager>(report, FileSystem);

	cog::verbs::verb_table verb_table;
	cog::compiler compiler(verb_table);

	game::screen::presenter_mapper ScreenPresenterMapper;
	place::place_controller<game::screen::place> ScreenPlaceController(event_bus, ScreenPresenterMapper);

	game::world::presenter_mapper WorldPresenterMapper;
	place::place_controller<game::world::place> WorldPlaceController(event_bus, WorldPresenterMapper);

	game::view_frame ScreenViewFrame(Window);
	game::view_frame WorldViewFrame(Window);

	game::screen::action::action_view ActionView;

	game::world::nothing::nothing_view nothing_view;
	game::world::level::level_view level_view(*systemcontentmanager);

	game::components Components(report, event_bus, Window, FileSystem, verb_table, compiler,
			ScreenPlaceController, WorldPlaceController, ScreenViewFrame, WorldViewFrame,
			ActionView, nothing_view, level_view);

	ScreenPresenterMapper.set_components(Components);
	WorldPresenterMapper.set_components(Components);

	// Register verbs
	game::world::level::level_presenter::register_verbs(verb_table, Components);

	bool running = true;

	// Register core event handlers
	event_bus.add_handler<events::print_event>([](events::print_event& e) {
		std::cout << e.message << std::endl;
	});

	event_bus.add_handler<events::exit_event>([&event_bus, &running](events::exit_event& e) {
		// A component has requested an application exit.
		// Check if components can quit.
		events::exiting_event exitingEvent;
		event_bus.fire_event(exitingEvent);

		if(exitingEvent.cancel) {
			return;
		}

		// All components can quit. Request shutdown.
		events::shutdown_event shutdownEvent;
		event_bus.fire_event(shutdownEvent);

		running = false;
	});

	// HACK: Set current episode to The Force Within.
	for(size_t i = 0; i < FileSystem.get_episode_count(); ++i) {
		if(boost::iequals(FileSystem.get_episode(i).get_episode_name(), "The Force Within")) {
			FileSystem.set_episode(i);
		}
	}

	// HACK: Set current level to 01narshadda.jkl.
	auto contentmanager = std::make_shared<content::manager>(report, FileSystem);
	const auto& lev = contentmanager->load<content::assets::level>("01narshadda.jkl", compiler);
	WorldPlaceController.go_to(game::world::level::level_place(contentmanager, lev));

	ScreenPlaceController.go_to(game::screen::action::action_place());
	// END HACK

	// game loop:
	sf::Clock clock;
	double gameplayAccumulator = 0.0;
	double gameplayElapsedTime = 0.0;

	sf::Event event;
	while(running) {
		while(Window.pollEvent(event)) {
			switch(event.type) {
			case sf::Event::Closed: {
				events::exit_event exitEvent;
				event_bus.fire_event(exitEvent);
			}
			break;

			case sf::Event::Resized:
				glViewport(0, 0, event.size.width, event.size.height);
				break;

			case sf::Event::LostFocus: {
				game::events::window_focus_event focusEvent(false);
				event_bus.fire_event(focusEvent);
			}
			break;

			case sf::Event::GainedFocus: {
				game::events::window_focus_event focusEvent(true);
				event_bus.fire_event(focusEvent);
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

		while(gameplayAccumulator >= gameplay_tick) {
			gameplayAccumulator -= gameplay_tick;

			// update simulation.
			WorldViewFrame.update(gameplay_tick);
			ScreenViewFrame.update(gameplay_tick);
		}

		// TODO: Render here
		glClearDepth(1.0f);
		glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		WorldViewFrame.draw(currentTime);
		ScreenViewFrame.draw(currentTime);

		Window.display();
	}

	return 0;
}
