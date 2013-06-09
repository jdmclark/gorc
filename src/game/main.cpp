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
#include <set>

const double gameplayTick = (1.0 / 60.0);

using namespace Gorc;
using namespace Gorc::Math;

Vector<3> CameraPosition = Zero<3>();
Vector<3> CameraLook = Vec(0.0f, 1.0f, 0.0f);
Vector<3> CameraUp = Vec(0.0f, 0.0f, 1.0f);
float CameraRadius = 0.125f;
size_t CameraCurrentSector = 104;

Vector<3> ds;
std::set<int> pcs;

class CollisionEvent {
public:
	float dist;

	virtual ~CollisionEvent() {
		return;
	}

	virtual void Apply() = 0;
};

class NullCollisionEvent : public CollisionEvent {
public:
	virtual void Apply() override {
		CameraPosition += ds;
		ds = Zero<3>();
	}
} nullEvent;

class SectorTransitEvent : public CollisionEvent {
public:
	Vector<3> StopPoint;
	Vector<3> ResidualDisplacement;
	size_t NextCameraSector;

	virtual void Apply() override {
		CameraPosition = StopPoint;
		ds = ResidualDisplacement;
		CameraCurrentSector = NextCameraSector;

		std::cout << "Moved to sector " << NextCameraSector << std::endl;
	}
} sectorTransitEvent;

class PhysicalCollisionEvent : public CollisionEvent {
public:
	Vector<3> StopPoint;
	Vector<3> ResidualDisplacement;

	virtual void Apply() override {
		CameraPosition = StopPoint;
		ds = ResidualDisplacement;
	}
};

class SurfaceCollisionEvent : public PhysicalCollisionEvent {
} surfaceCollisionEvent;

class EdgeCollisionEvent : public PhysicalCollisionEvent {
} edgeCollisionEvent;

class VertexCollisionEvent : public PhysicalCollisionEvent {
} vertexCollisionEvent;

CollisionEvent* CurrentCollisionEvent;

void StepCameraPhysicsSurface(const Gorc::Content::Assets::Level& lev, const Gorc::Content::Assets::LevelSector& sec,
		const Gorc::Content::Assets::LevelSurface& surf) {
}

void StepCameraPhysicsSector(const Gorc::Content::Assets::Level& lev, const Gorc::Content::Assets::LevelSector& sec);

void StepCameraPhysicsAdjoin(const Gorc::Content::Assets::Level& lev, const Gorc::Content::Assets::LevelSector& sec,
		const Gorc::Content::Assets::LevelSurface& surf) {
	if(surf.AdjoinedSector == CameraCurrentSector || Dot(surf.Normal, ds) > 0.0f) {
		return;
	}

	Vector<3> p = lev.Vertices[std::get<0>(surf.Vertices[0])];

	float u = Dot(surf.Normal, p - CameraPosition) / Dot(surf.Normal, ds);
	if(u < 0.0f || u > 1.0f) {
		return;
	}

	// Check if point of intersection is inside polygon.
	Vector<3> residual_displacement = u * ds;
	Vector<3> stop_point = CameraPosition + residual_displacement;

	Vector<3> prev_vx = lev.Vertices[std::get<0>(surf.Vertices[surf.Vertices.size() - 1])];
	for(int i = 0; i < surf.Vertices.size(); ++i) {
		Vector<3> next_vx = lev.Vertices[std::get<0>(surf.Vertices[i])];

		Vector<3> edge_cross = Cross(next_vx - prev_vx, surf.Normal);
		if(Dot(edge_cross, stop_point - prev_vx) > 0.0f) {
			return;
		}

		prev_vx = next_vx;
	}

	float dist = Length2(residual_displacement);
	if(dist <= CurrentCollisionEvent->dist) {
		sectorTransitEvent.NextCameraSector = surf.AdjoinedSector;
		sectorTransitEvent.ResidualDisplacement = residual_displacement;
		sectorTransitEvent.StopPoint = stop_point;
		sectorTransitEvent.dist = dist;
		CurrentCollisionEvent = &sectorTransitEvent;
	}
}

void StepCameraPhysicsSector(const Gorc::Content::Assets::Level& lev, const Gorc::Content::Assets::LevelSector& sec) {
	for(size_t i = sec.FirstSurface; i < sec.FirstSurface + sec.SurfaceCount; ++i) {
		const auto& surf = lev.Surfaces[i];

		if(surf.Adjoin >= 0 && !(surf.Flags & Gorc::Content::Assets::SurfaceFlag::Impassable)) {
			StepCameraPhysicsAdjoin(lev, sec, surf);
		}
		else {
			StepCameraPhysicsSurface(lev, sec, surf);
		}
	}
}

void CameraBroadphaseVisitSector(const Gorc::Content::Assets::Level& lev, const Gorc::Content::Assets::LevelSector& sec, float max_dist) {
	for(size_t i = sec.FirstSurface; i < sec.FirstSurface + sec.SurfaceCount; ++i) {
		const auto& surf = lev.Surfaces[i];

		if(surf.Adjoin >= 0 && !(surf.Flags & Gorc::Content::Assets::SurfaceFlag::Impassable)) {
			if(pcs.find(surf.AdjoinedSector) == pcs.end()) {
				float dist = Dot(CameraPosition - lev.Vertices[std::get<0>(surf.Vertices[0])], surf.Normal);
				if(dist >= 0.0f && dist <= max_dist) {
					pcs.insert(surf.AdjoinedSector);
					CameraBroadphaseVisitSector(lev, lev.Sectors[surf.AdjoinedSector], max_dist);
				}
			}
		}
	}
}

void CameraBroadphase(const Gorc::Content::Assets::Level& lev) {
	float max_dist = Length(ds) + CameraRadius + CameraRadius;
	pcs.clear();
	pcs.insert(CameraCurrentSector);
	CameraBroadphaseVisitSector(lev, lev.Sectors[CameraCurrentSector], max_dist);
}

void StepCameraPhysics(const Gorc::Content::Assets::Level& lev) {
	CurrentCollisionEvent = &nullEvent;
	CurrentCollisionEvent->dist = Length2(ds);

	for(int i : pcs) {
		StepCameraPhysicsSector(lev, lev.Sectors[i]);
	}

	CurrentCollisionEvent->Apply();
}

void TranslateCamera(const Vector<3>& amt, const Gorc::Content::Assets::Level& lev) {
	ds = Zero<3>();
	ds += Get<X>(amt) * Cross(CameraLook, CameraUp);
	ds += Get<Z>(amt) * CameraUp;
	ds += Get<Y>(amt) * CameraLook;

	// Perform collision detection against world.
	CameraBroadphase(lev);
	while(Length2(ds) > std::numeric_limits<float>::epsilon()) {
		StepCameraPhysics(lev);
	}
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
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

				if(sector.Number == CameraCurrentSector) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glDisable(GL_TEXTURE_2D);

					glBegin(GL_TRIANGLES);

					Vector<3> first_geo = level.Vertices[std::get<0>(surface.Vertices[0])];
					Vector<2> first_tex = level.TextureVertices[std::get<1>(surface.Vertices[0])];

					for(size_t i = 2; i < surface.Vertices.size(); ++i) {
						Vector<3> second_geo = level.Vertices[std::get<0>(surface.Vertices[i - 1])];
						Vector<2> second_tex = level.TextureVertices[std::get<1>(surface.Vertices[i - 1])];

						Vector<3> third_geo = level.Vertices[std::get<0>(surface.Vertices[i])];
						Vector<2> third_tex = level.TextureVertices[std::get<1>(surface.Vertices[i])];

						glTexCoord2f(Get<X>(first_tex) * Get<X>(tex_scale), Get<Y>(first_tex) * Get<Y>(tex_scale));
						glColor4f(1.0f, 1.0f, 0.6f, 1.0f);
						glVertex3f(Get<X>(first_geo), Get<Y>(first_geo), Get<Z>(first_geo));

						glTexCoord2f(Get<X>(second_tex) * Get<X>(tex_scale), Get<Y>(second_tex) * Get<Y>(tex_scale));
						glColor4f(1.0f, 1.0f, 0.6f, 1.0f);
						glVertex3f(Get<X>(second_geo), Get<Y>(second_geo), Get<Z>(second_geo));

						glTexCoord2f(Get<X>(third_tex) * Get<X>(tex_scale), Get<Y>(third_tex) * Get<Y>(tex_scale));
						glColor4f(1.0f, 1.0f, 0.6f, 1.0f);
						glVertex3f(Get<X>(third_geo), Get<Y>(third_geo), Get<Z>(third_geo));
					}

					glEnd();

					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glEnable(GL_TEXTURE_2D);
				}
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

	// Set camera position to centroid of current sector.
	CameraPosition = lev.Sectors[CameraCurrentSector].Center;

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
				TranslateCamera(Translate, lev);
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
