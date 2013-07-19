#include "content/assets/shader.h"

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
#include "cog/compiler.h"

#include <iostream>

const double gameplayTick = (1.0 / 120.0);

void RegisterLevelVerbs(Gorc::Cog::Verbs::VerbTable& verbTable, Gorc::Game::Components& components) {
	// Anim / Cel verbs
	verbTable.AddVerb<int, 3>("surfaceanim", [&components](int surface, float rate, int flags) {
		return components.CurrentLevelPresenter->SurfaceAnim(surface, rate, Gorc::FlagSet<Gorc::Content::Assets::SurfaceAnimationFlag>(flags));
	});

	verbTable.AddVerb<int, 1>("getsurfaceanim", [&components](int surface) { return components.CurrentLevelPresenter->GetSurfaceAnim(surface); });
	verbTable.AddVerb<void, 1>("stopanim", [&components](int anim) { components.CurrentLevelPresenter->StopAnim(anim); });
	verbTable.AddVerb<void, 1>("stopsurfaceanim", [&components](int surface) {
		components.CurrentLevelPresenter->StopAnim(components.CurrentLevelPresenter->GetSurfaceAnim(surface));
	});

	verbTable.AddVerb<int, 1>("getwallcel", [&components](int surface) { return components.CurrentLevelPresenter->GetSurfaceCel(surface); });
	verbTable.AddVerb<int, 2>("setwallcel", [&components](int surface, int cel) { components.CurrentLevelPresenter->SetSurfaceCel(surface, cel); return 1; });

	// Frame verbs
	verbTable.AddVerb<int, 1>("getcurframe", [&components](int thing) { return components.CurrentLevelPresenter->GetCurFrame(thing); });
	verbTable.AddVerb<void, 3>("movetoframe", [&components](int thing, int frame, float speed) {
		return components.CurrentLevelPresenter->MoveToFrame(thing, frame, speed); });

	// Message verbs
	verbTable.AddVerb<int, 1>("getparam", [&components](int param_num) { return components.CurrentLevelPresenter->GetParam(param_num); });
	verbTable.AddVerb<int, 0>("getsenderid", [&components]{ return components.CurrentLevelPresenter->GetSenderId(); });
	verbTable.AddVerb<int, 0>("getsenderref", [&components]{ return components.CurrentLevelPresenter->GetSenderRef(); });
	verbTable.AddVerb<int, 0>("getsendertype", [&components]{ return components.CurrentLevelPresenter->GetSenderType(); });
	verbTable.AddVerb<int, 0>("getsourceref", [&components]{ return components.CurrentLevelPresenter->GetSourceRef(); });
	verbTable.AddVerb<int, 0>("getsourcetype", [&components]{ return components.CurrentLevelPresenter->GetSourceType(); });
	verbTable.AddVerb<void, 1>("setpulse", [&components](float time) { components.CurrentLevelPresenter->SetPulse(time); });
	verbTable.AddVerb<void, 1>("settimer", [&components](float time) { components.CurrentLevelPresenter->SetTimer(time); });
	verbTable.AddVerb<void, 1>("sleep", [&components](float time) { components.CurrentLevelPresenter->Sleep(time); });

	// Options verbs
	verbTable.AddVerb<int, 0>("getdifficulty", [&components] {
		// TODO: Add actual difficulty setting.
		return static_cast<int>(Gorc::Content::Assets::Difficulty::Medium);
	});

	// Player verbs
	verbTable.AddVerb<int, 0>("getlocalplayerthing", [&components] { return components.CurrentLevelPresenter->GetLocalPlayerThing(); });

	// Print verbs
	verbTable.AddVerb<void, 2>("jkprintunistring", [&components](int destination, const char* message) {
		// TODO: Add actual jkPrintUniString once localization is implemented.
		std::cout << message << std::endl;
	});

	// Sector verbs
	verbTable.AddVerb<void, 2>("setsectoradjoins", [&components](int sector_id, bool state) {
		components.CurrentLevelPresenter->SetSectorAdjoins(sector_id, state);
	});

	verbTable.AddVerb<void, 3>("setsectorlight", [&components](int sector_id, float light, float delay) {
		components.CurrentLevelPresenter->SetSectorLight(sector_id, light, delay);
	});

	verbTable.AddVerb<void, 2>("sectoradjoins", [&components](int sector_id, bool state) {
		components.CurrentLevelPresenter->SetSectorAdjoins(sector_id, state);
	});

	verbTable.AddVerb<void, 3>("sectorlight", [&components](int sector_id, float light, float delay) {
		components.CurrentLevelPresenter->SetSectorLight(sector_id, light, delay);
	});

	// Sound verbs
	verbTable.AddVerb<void, 3>("playsong", [&components](int start, int end, int loopto) {
		components.CurrentLevelPresenter->PlaySong(start, end, loopto);
	});

	verbTable.AddVerb<int, 4>("playsoundlocal", [&components](int wav, float volume, float panning, int flags) {
		return components.CurrentLevelPresenter->PlaySoundLocal(wav, volume, panning, Gorc::FlagSet<Gorc::Content::Assets::SoundFlag>(flags));
	});

	verbTable.AddVerb<int, 6>("playsoundpos", [&components](int wav, Gorc::Math::Vector<3> pos, float volume, float min_rad, float max_rad, int flags) {
		return components.CurrentLevelPresenter->PlaySoundPos(wav, pos, volume, min_rad, max_rad, Gorc::FlagSet<Gorc::Content::Assets::SoundFlag>(flags));
	});

	verbTable.AddVerb<int, 6>("playsoundthing", [&components](int wav, int thing, float volume, float min_rad, float max_rad, int flags) {
		return components.CurrentLevelPresenter->PlaySoundThing(wav, thing, volume, min_rad, max_rad, Gorc::FlagSet<Gorc::Content::Assets::SoundFlag>(flags));
	});

	verbTable.AddVerb<void, 1>("setmusicvol", [&components](float vol) { components.CurrentLevelPresenter->SetMusicVol(vol); });

	// Surface verbs
	verbTable.AddVerb<Gorc::Math::Vector<3>, 1>("surfacecenter", [&components](int surface) {
		return components.CurrentLevelPresenter->GetSurfaceCenter(surface);
	});

	// System verbs
	verbTable.AddVerb<float, 0>("rand", []{ return sf::Randomizer::Random(0.0f, 1.0f); });

	// Thing action verbs
	verbTable.AddVerb<int, 2>("creatething", [&components](int tpl_id, int thing_pos) {
		return components.CurrentLevelPresenter->CreateThingAtThing(tpl_id, thing_pos);
	});

	verbTable.AddVerb<bool, 1>("isthingmoving", [&components](int thing_id) { return components.CurrentLevelPresenter->IsThingMoving(thing_id); });
	verbTable.AddVerb<bool, 1>("ismoving", [&components](int thing_id) { return components.CurrentLevelPresenter->IsThingMoving(thing_id); });

	// Thing property verbs
	verbTable.AddVerb<int, 1>("getthingsector", [&components](int thing_id) { return components.CurrentLevelPresenter->GetThingSector(thing_id); });
}

int main(int argc, char** argv) {
	// Create window and OpenGL context.
	sf::Window Window(sf::VideoMode(1280, 720, 32), "Gorc");
	Window.UseVerticalSync(true);
	Window.SetActive();

	// Initialize GLEW.
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		// TODO: Print error.
		return 1;
	}

	const sf::Input& Input = Window.GetInput();

	Gorc::Diagnostics::StreamReport Report(std::cout);
	Gorc::Event::EventBus EventBus;
	Gorc::Content::VFS::VirtualFileSystem FileSystem("game/restricted",
			"game/resource", "game/episode", Report);

	auto systemContentManager = std::make_shared<Gorc::Content::Manager>(Report, FileSystem);
	const auto& surfaceShader = systemContentManager->Load<Gorc::Content::Assets::Shader>("surface.glsl");

	Gorc::Cog::Verbs::VerbTable VerbTable;
	Gorc::Cog::Compiler Compiler(VerbTable);

	Gorc::Game::Screen::PresenterMapper ScreenPresenterMapper;
	Gorc::Place::PlaceController<Gorc::Game::Screen::Place> ScreenPlaceController(EventBus, ScreenPresenterMapper);

	Gorc::Game::World::PresenterMapper WorldPresenterMapper;
	Gorc::Place::PlaceController<Gorc::Game::World::Place> WorldPlaceController(EventBus, WorldPresenterMapper);

	Gorc::Game::ViewFrame ScreenViewFrame(Window);
	Gorc::Game::ViewFrame WorldViewFrame(Window);

	Gorc::Game::Screen::Action::ActionView ActionView;

	Gorc::Game::World::Nothing::NothingView NothingView;
	Gorc::Game::World::Level::LevelView LevelView(surfaceShader);

	Gorc::Game::Components Components(Report, EventBus, Input, FileSystem, VerbTable, Compiler,
			ScreenPlaceController, WorldPlaceController, ScreenViewFrame, WorldViewFrame,
			ActionView, NothingView, LevelView);

	ScreenPresenterMapper.SetComponents(Components);
	WorldPresenterMapper.SetComponents(Components);

	RegisterLevelVerbs(VerbTable, Components);

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
	const auto& lev = contentManager->Load<Gorc::Content::Assets::Level>("01narshadda.jkl", Compiler);
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

		while(gameplayAccumulator >= gameplayTick) {
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
