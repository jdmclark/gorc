#include "levelview.h"
#include "levelpresenter.h"
#include "game/components.h"
#include "levelmodel.h"
#include "content/assets/model.h"

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <GL/glu.h>

using namespace Gorc::Math;

void Gorc::Game::World::Level::LevelView::Update(double dt) {
	if(currentPresenter) {
		currentPresenter->Update(dt);
	}
}

void Gorc::Game::World::Level::LevelView::Draw(double dt, const Math::Box<2, unsigned int>& view_size) {
	if(currentModel) {
		double width = static_cast<double>(view_size.Size<X>());
		double height = static_cast<double>(view_size.Size<Y>());
		double aspect = width / height;
		DrawLevel(aspect);

		for(const auto& thing : currentModel->Things) {
			DrawThing(thing, aspect);
		}

		currentModel->DynamicsWorld.setDebugDrawer(&physicsDebugDraw);
		//currentModel->DynamicsWorld.debugDrawWorld();
	}
}

void Gorc::Game::World::Level::LevelView::DrawThing(const Thing& thing, double aspect) {
	if(!thing.Model3d) {
		return;
	}

	glDisable(GL_CULL_FACE);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	btTransform trns;
	thing.RigidBody->getMotionState()->getWorldTransform(trns);

	float mat[16];
	trns.getOpenGLMatrix(mat);

	glMultMatrixf(mat);

	const Content::Assets::Model& model = *thing.Model3d;

	const Content::Assets::ModelGeoSet& geoset = model.GeoSets[0];
	for(const auto& mesh : geoset.Meshes) {
		for(const auto& face : mesh.Faces) {
			if(face.Material >= 0) {
				const auto& material = model.Materials[face.Material];

				float alpha = (face.Type & Content::Assets::FaceTypeFlag::Translucent) ? 0.5f : 1.0f;

				Vector<2> tex_scale = Vec(1.0f / static_cast<float>(material->Width),
						1.0f / static_cast<float>(material->Height));

				material->Cels[0].Diffuse->Bind();
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				glBegin(GL_TRIANGLES);

				Vector<3> first_geo = mesh.Vertices[std::get<0>(face.Vertices[0])];
				Vector<2> first_tex = mesh.TextureVertices[std::get<1>(face.Vertices[0])];
				float first_intensity = 1.0f;

				for(size_t i = 2; i < face.Vertices.size(); ++i) {
					Vector<3> second_geo = mesh.Vertices[std::get<0>(face.Vertices[i - 1])];
					Vector<2> second_tex = mesh.TextureVertices[std::get<1>(face.Vertices[i - 1])];
					float second_intensity = 1.0f;

					Vector<3> third_geo = mesh.Vertices[std::get<0>(face.Vertices[i])];
					Vector<2> third_tex = mesh.TextureVertices[std::get<1>(face.Vertices[i])];
					float third_intensity = 1.0f;

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

	glPopMatrix();
}

void Gorc::Game::World::Level::LevelView::DrawLevel(double aspect) {
	const Content::Assets::Level& lev = currentModel->Level;

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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Vector<3> CamLookPoint = currentModel->CameraPosition + currentModel->CameraLook;
	gluLookAt(Get<X>(currentModel->CameraPosition), Get<Y>(currentModel->CameraPosition), Get<Z>(currentModel->CameraPosition),
			Get<X>(CamLookPoint), Get<Y>(CamLookPoint), Get<Z>(CamLookPoint),
			Get<X>(currentModel->CameraUp), Get<Y>(currentModel->CameraUp), Get<Z>(currentModel->CameraUp));

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0, aspect, 0.001, 1000.0);

	for(const auto& sector : lev.Sectors) {
		for(size_t i = sector.FirstSurface; i < sector.SurfaceCount + sector.FirstSurface; ++i) {
			const auto& surface = lev.Surfaces[i];
			if(surface.Material >= 0) {

				const auto& material_entry = lev.MaterialEntries[surface.Material];
				const auto& material = lev.Materials[surface.Material];

				float alpha = (surface.FaceTypeFlags & Content::Assets::FaceTypeFlag::Translucent) ? 0.5f : 1.0f;

				Vector<2> tex_scale = Vec(1.0f / static_cast<float>(material->Width),
						1.0f / static_cast<float>(material->Height));

				material->Cels[0].Diffuse->Bind();
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				glBegin(GL_TRIANGLES);

				Vector<3> first_geo = lev.Vertices[std::get<0>(surface.Vertices[0])];
				Vector<2> first_tex = lev.TextureVertices[std::get<1>(surface.Vertices[0])];
				float first_intensity = std::get<2>(surface.Vertices[0]) + sector.ExtraLight + surface.ExtraLight;

				for(size_t i = 2; i < surface.Vertices.size(); ++i) {
					Vector<3> second_geo = lev.Vertices[std::get<0>(surface.Vertices[i - 1])];
					Vector<2> second_tex = lev.TextureVertices[std::get<1>(surface.Vertices[i - 1])];
					float second_intensity = std::get<2>(surface.Vertices[i - 1]) + sector.ExtraLight + surface.ExtraLight;

					Vector<3> third_geo = lev.Vertices[std::get<0>(surface.Vertices[i])];
					Vector<2> third_tex = lev.TextureVertices[std::get<1>(surface.Vertices[i])];
					float third_intensity = std::get<2>(surface.Vertices[i]) + sector.ExtraLight + surface.ExtraLight;

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

				/*if(sector.Number == CameraCurrentSector) {
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
				}*/
			}
		}
	}
}

void Gorc::Game::World::Level::LevelView::PhysicsDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);

	glColor3fv(color.m_floats);
	glVertex3fv(from.m_floats);

	glColor3fv(color.m_floats);
	glVertex3fv(to.m_floats);

	glEnd();
	return;
}

void Gorc::Game::World::Level::LevelView::PhysicsDebugDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB,
		btScalar distance, int lifeTime, const btVector3& color) {
	return;
}

void Gorc::Game::World::Level::LevelView::PhysicsDebugDraw::reportErrorWarning(const char* warningString) {
	return;
}

void Gorc::Game::World::Level::LevelView::PhysicsDebugDraw::draw3dText(const btVector3& location, const char* textString) {
	return;
}

void Gorc::Game::World::Level::LevelView::PhysicsDebugDraw::setDebugMode(int debugMode) {
	return;
}

int Gorc::Game::World::Level::LevelView::PhysicsDebugDraw::getDebugMode() const {
	return DBG_DrawWireframe | DBG_DrawContactPoints;
}

void Gorc::Game::World::Level::LevelView::TranslateCamera(const Math::Vector<3>& amt) {
	currentPresenter->TranslateCamera(amt);
}

void Gorc::Game::World::Level::LevelView::YawCamera(double amt) {
	currentPresenter->YawCamera(amt);
}

void Gorc::Game::World::Level::LevelView::PitchCamera(double amt) {
	currentPresenter->PitchCamera(amt);
}
