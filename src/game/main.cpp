#include "framework/event/eventbus.h"
#include "framework/events/printevent.h"
#include "framework/events/exitingevent.h"
#include "framework/events/exitevent.h"
#include "framework/events/shutdownevent.h"
#include <iostream>
#include <cmath>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "framework/diagnostics/streamreport.h"
#include "content/vfs/virtualfilesystem.h"
#include "content/manager.h"

#include "content/assets/colormap.h"
#include "content/assets/material.h"

const double gameplayTick = (1.0 / 60.0);

int main(int argc, char** argv) {
	sf::Window Window(sf::VideoMode(1280, 720, 32), "Gorc");

	Gorc::Event::EventBus eventBus;

	// Create temporary content manager and virtual file system
	Gorc::Diagnostics::StreamReport Report(std::cout);
	Gorc::Content::VFS::VirtualFileSystem vfs("game/restricted",
			"game/resource", "game/episode", Report);
	Gorc::Content::Manager manager(Report, vfs);

	// Temporary: load DFLT.MAT for display.
	const auto& dflt_cmp = manager.Load<Gorc::Content::Assets::Colormap>("dflt.cmp");
	const auto& dflt_mat = manager.Load<Gorc::Content::Assets::Material>("dflt.mat", dflt_cmp);

	bool running = true;

	// Register core event handlers
	eventBus.AddHandler<Gorc::Events::PrintEvent>([](Gorc::Events::PrintEvent& e) {
		std::cout << e.message << std::endl;
	});

	eventBus.AddHandler<Gorc::Events::ExitEvent>([&eventBus, &running](Gorc::Events::ExitEvent& e) {
		// A component has requested an application exit.
		// Check if components can quit.
		Gorc::Events::ExitingEvent exitingEvent;
		eventBus.FireEvent(exitingEvent);

		if(exitingEvent.Cancel) {
			return;
		}

		// All components can quit. Request shutdown.
		Gorc::Events::ShutdownEvent shutdownEvent;
		eventBus.FireEvent(shutdownEvent);

		running = false;
	});

	// HACK: Texture
	unsigned int texture_id;
	glGenTextures(1, &texture_id);

	// Game loop:
	sf::Clock clock;
	double gameplayAccumulator = 0.0;
	double gameplayElapsedTime = 0.0;

	sf::Event event;
	while(running) {
		while(Window.GetEvent(event)) {
			if(event.Type == sf::Event::Closed) {
				Gorc::Events::ExitEvent exitEvent;
				eventBus.FireEvent(exitEvent);
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

			// TODO: Update simulation here
		}

		// TODO: Render here
		glClearDepth(1.0f);
		glClearColor(0.392f, 0.584f, 0.929f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		glEnable(GL_TEXTURE_2D);
		dflt_mat.Cels[0].Diffuse->Bind();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, Window.GetWidth(), Window.GetHeight(), 0, -1, 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBegin(GL_QUADS);

		glVertex3f(100.0f, 100.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f);

		glVertex3f(500.0f, 100.0f, 0.0f);
		glTexCoord2f(5.0f, 0.0f);

		glVertex3f(500.0f, 500.0f, 0.0f);
		glTexCoord2f(5.0f, 5.0f);

		glVertex3f(100.0f, 500.0f, 0.0f);
		glTexCoord2f(0.0f, 5.0f);

		glEnd();

		Window.Display();
	}

	return 0;
}
