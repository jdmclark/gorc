#include "framework/event/eventbus.h"
#include "framework/events/printevent.h"
#include "framework/events/exitingevent.h"
#include "framework/events/exitevent.h"
#include "framework/events/shutdownevent.h"
#include <iostream>
#include <cmath>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <boost/algorithm/string/predicate.hpp>

#include "framework/diagnostics/streamreport.h"
#include "content/vfs/virtualfilesystem.h"
#include "content/manager.h"

#include "content/assets/level.h"

#include <GL/glu.h>

const double gameplayTick = (1.0 / 60.0);

using namespace Gorc;
using namespace Gorc::Math;

Vector<3> CameraPosition = Zero<3>();
Vector<3> CameraLook = Vec(0.0f, 1.0f, 0.0f);
Vector<3> CameraUp = Vec(0.0f, 0.0f, 1.0f);

void TranslateCamera(const Vector<3>& amt) {
	CameraPosition += Get<X>(amt) * Cross(CameraLook, CameraUp);
	CameraPosition += Get<Z>(amt) * CameraUp;
	CameraPosition += Get<Y>(amt) * CameraLook;
}

void YawCamera(double amt) {
	float sint = std::sin(amt);
	float cost = std::cos(amt);

	Vector<3> NewLook = Vec(
			cost * Get<X>(CameraLook) - sint * Get<Y>(CameraLook),
			sint * Get<X>(CameraLook) + cost * Get<Y>(CameraLook),
			Get<Z>(CameraLook));
	Vector<3> NewUp = Vec(
			cost * Get<X>(CameraUp) - sint * Get<Y>(CameraUp),
			sint * Get<X>(CameraUp) + cost * Get<Y>(CameraUp),
			Get<Z>(CameraUp));

	CameraLook = Normalize(NewLook);
	CameraUp = Normalize(NewUp);
}

void PitchCamera(double amt) {
	float sint = std::sin(amt);
	float cost = std::cos(amt);

	Vector<3> NewUp = (-sint * CameraLook) + (cost * CameraUp);
	Vector<3> NewLook = (cost * CameraLook) + (sint * CameraUp);
	CameraUp = Normalize(NewUp);
	CameraLook = Normalize(NewLook);
}

void DrawLevel(const Gorc::Content::Assets::Level& level, double aspect) {
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Vector<3> CamLookPoint = CameraPosition + CameraLook;
	gluLookAt(Get<X>(CameraPosition), Get<Y>(CameraPosition), Get<Z>(CameraPosition),
			Get<X>(CamLookPoint), Get<Y>(CamLookPoint), Get<Z>(CamLookPoint),
			Get<X>(CameraUp), Get<Y>(CameraUp), Get<Z>(CameraUp));

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0, aspect, 0.001, 1000.0);

	for(const auto& sector : level.Sectors) {
		for(size_t i = sector.FirstSurface; i < sector.SurfaceCount + sector.FirstSurface; ++i) {
			const auto& surface = level.Surfaces[i];
			if(surface.Material >= 0) {
				const auto& material_entry = level.MaterialEntries[surface.Material];
				const auto& material = level.Materials[surface.Material];

				float alpha = (surface.FaceTypeFlags & Content::Assets::FaceTypeFlag::Translucent) ? 0.5f : 1.0f;

				Vector<2> tex_scale = Vec(std::get<1>(material_entry) / static_cast<float>(material->Width),
						std::get<2>(material_entry) / static_cast<float>(material->Height));

				material->Cels[0].Diffuse->Bind();

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

				glBegin(GL_TRIANGLES);

				Vector<3> first_geo = level.Vertices[std::get<0>(surface.Vertices[0])];
				Vector<2> first_tex = level.TextureVertices[std::get<1>(surface.Vertices[0])];
				float first_intensity = std::get<2>(surface.Vertices[0]) + sector.AmbientLight + sector.ExtraLight + surface.ExtraLight;

				for(size_t i = 2; i < surface.Vertices.size(); ++i) {
					Vector<3> second_geo = level.Vertices[std::get<0>(surface.Vertices[i - 1])];
					Vector<2> second_tex = level.TextureVertices[std::get<1>(surface.Vertices[i - 1])];
					float second_intensity = std::get<2>(surface.Vertices[i - 1]) + sector.AmbientLight + sector.ExtraLight + surface.ExtraLight;

					Vector<3> third_geo = level.Vertices[std::get<0>(surface.Vertices[i])];
					Vector<2> third_tex = level.TextureVertices[std::get<1>(surface.Vertices[i])];
					float third_intensity = std::get<2>(surface.Vertices[i]) + sector.AmbientLight + sector.ExtraLight + surface.ExtraLight;

					glTexCoord2f(Get<X>(first_tex) * Get<X>(tex_scale), Get<Y>(first_tex) * Get<Y>(tex_scale));
					glColor4f(first_intensity, first_intensity, first_intensity, alpha);
					glVertex3f(Get<X>(first_geo), Get<Y>(first_geo), Get<Z>(first_geo));

					glTexCoord2f(Get<X>(second_tex) * Get<X>(tex_scale), Get<Y>(second_tex) * Get<Y>(tex_scale));
					glColor4f(second_intensity, second_intensity, second_intensity, alpha);
					glVertex3f(Get<X>(second_geo), Get<Y>(second_geo), Get<Z>(second_geo));

					glTexCoord2f(Get<X>(third_tex) * Get<X>(tex_scale), Get<Y>(third_tex) * Get<Y>(tex_scale));
					glColor4f(third_intensity, third_intensity, third_intensity, alpha);
					glVertex3f(Get<X>(third_geo), Get<Y>(third_geo), Get<Z>(third_geo));
				}

				glEnd();
			}
		}
	}
}

int main(int argc, char** argv) {
	sf::Window Window(sf::VideoMode(1280, 720, 32), "Gorc");
	Gorc::Event::EventBus eventBus;
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

	// HACK: Load test assets
	Gorc::Diagnostics::StreamReport Report(std::cout);
	Gorc::Content::VFS::VirtualFileSystem vfs("game/restricted",
			"game/resource", "game/episode", Report);
	Gorc::Content::Manager manager(Report, vfs);

	// Temporary: load 01narshadda.jkl for display.
	for(size_t i = 0; i < vfs.GetEpisodeCount(); ++i) {
		if(boost::iequals(vfs.GetEpisode(i).GetEpisodeName(), "The Force Within")) {
			vfs.SetEpisode(i);
		}
	}

	const auto& lev = manager.Load<Gorc::Content::Assets::Level>("01narshadda.jkl");

	const sf::Input& input = Window.GetInput();
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
			if(input.IsKeyDown(sf::Key::Escape)) {
				Gorc::Events::ExitEvent evt;
				eventBus.FireEvent(evt);
			}

			Vector<3> Translate = Zero<3>();
			if(input.IsKeyDown(sf::Key::W)) {
				Translate += Vec(0.0f, 1.0f, 0.0f);
			}

			if(input.IsKeyDown(sf::Key::A)) {
				Translate += Vec(-1.0f, 0.0f, 0.0f);
			}

			if(input.IsKeyDown(sf::Key::S)) {
				Translate += Vec(0.0f, -1.0f, 0.0f);
			}

			if(input.IsKeyDown(sf::Key::D)) {
				Translate += Vec(1.0f, 0.0f, 0.0f);
			}

			if(input.IsKeyDown(sf::Key::Q)) {
				YawCamera(gameplayTick * 2.0f);
			}

			if(input.IsKeyDown(sf::Key::E)) {
				YawCamera(-gameplayTick * 2.0f);
			}

			if(input.IsKeyDown(sf::Key::PageUp)) {
				PitchCamera(-gameplayTick * 1.5f);
			}

			if(input.IsKeyDown(sf::Key::PageDown)) {
				PitchCamera(gameplayTick * 1.5f);
			}

			if(Length(Translate) > std::numeric_limits<float>::epsilon()) {
				Translate = Normalize(Translate);

				Translate *= gameplayTick;
				TranslateCamera(Translate);
			}
		}

		Window.SetActive();

		// TODO: Render here
		glClearDepth(1.0f);
		glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DrawLevel(lev, static_cast<double>(Window.GetWidth()) / static_cast<double>(Window.GetHeight()));

		Window.Display();
	}

	return 0;
}
