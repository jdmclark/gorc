#include "content/assets/shader.h"
#include "levelview.h"
#include "levelpresenter.h"
#include "game/components.h"
#include "levelmodel.h"
#include "content/assets/model.h"

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <GL/glu.h>

using namespace Gorc::Math;

Gorc::Game::World::Level::LevelView::LevelView(const Content::Assets::Shader& surfaceShader)
	: surfaceShader(surfaceShader), currentPresenter(nullptr), currentModel(nullptr) {
	return;
}

void Gorc::Game::World::Level::LevelView::Update(double dt) {
	if(currentPresenter) {
		currentPresenter->Update(dt);
	}
}

void Gorc::Game::World::Level::LevelView::Draw(double dt, const Math::Box<2, unsigned int>& view_size) {
	if(currentModel) {
		glUseProgram(surfaceShader.program);

		unsigned int current_camera_sector_id = currentModel->Things[currentModel->CameraThingId].Sector;
		const auto& current_camera_sector = currentModel->Sectors[current_camera_sector_id];

		std::array<float, 4> tint_color;
		const auto& current_sector_tint = current_camera_sector.Tint;
		std::get<0>(tint_color) = Math::Get<0>(current_sector_tint);
		std::get<1>(tint_color) = Math::Get<1>(current_sector_tint);
		std::get<2>(tint_color) = Math::Get<2>(current_sector_tint);
		std::get<3>(tint_color) = Math::Length(current_sector_tint);

		auto diffuse_ul = glGetUniformLocation(surfaceShader.program, "diffuse");
		glUniform1i(diffuse_ul, 0);
		auto light_ul = glGetUniformLocation(surfaceShader.program, "light");
		glUniform1i(light_ul, 1);
		auto sector_tint_ul = glGetUniformLocation(surfaceShader.program, "sector_tint");
		glUniform4fv(sector_tint_ul, 4, tint_color.data());

		double width = static_cast<double>(view_size.Size<X>());
		double height = static_cast<double>(view_size.Size<Y>());
		double aspect = width / height;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(70.0, aspect, 0.001, 1000.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		Thing& cameraThing = currentModel->Things[currentModel->CameraThingId];
		btTransform trns;
		cameraThing.RigidBody->getMotionState()->getWorldTransform(trns);

		Vector<3> CamPosition = Math::VecBt(trns.getOrigin());
		Vector<3> CamLookPoint = CamPosition + currentModel->CameraLook;
		gluLookAt(Get<X>(CamPosition), Get<Y>(CamPosition), Get<Z>(CamPosition),
				Get<X>(CamLookPoint), Get<Y>(CamLookPoint), Get<Z>(CamLookPoint),
				Get<X>(currentModel->CameraUp), Get<Y>(currentModel->CameraUp), Get<Z>(currentModel->CameraUp));

		std::array<double, 16> proj_matrix;
		std::array<double, 16> view_matrix;
		std::array<int, 4> viewport;

		glGetDoublev(GL_PROJECTION_MATRIX, proj_matrix.data());
		glGetDoublev(GL_MODELVIEW_MATRIX, view_matrix.data());
		glGetIntegerv(GL_VIEWPORT, viewport.data());

		Math::Box<2, double> adj_bbox(Math::Vec<double>(static_cast<double>(std::get<0>(viewport)), static_cast<double>(std::get<1>(viewport))),
				Math::Vec<double>(static_cast<double>(std::get<0>(viewport) + std::get<2>(viewport)),
						static_cast<double>(std::get<1>(viewport) + std::get<3>(viewport))));

		sector_visited_scratch.clear();
		sector_vis_scratch.clear();
		sector_vis_scratch.emplace(current_camera_sector_id);
		DoSectorVis(current_camera_sector_id, proj_matrix, view_matrix, viewport, adj_bbox, cameraThing.Position, currentModel->CameraLook);

		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		DrawLevel(CamPosition);

		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		for(const auto& thing : currentModel->Things) {
			DrawThing(thing);
		}

		glEnable(GL_CULL_FACE);

		for(auto surf_tuple : translucent_surfaces_scratch) {
			DrawSurfaceTranslucent(std::get<1>(surf_tuple), currentModel->Sectors[std::get<0>(surf_tuple)]);
		}

		currentModel->DynamicsWorld.setDebugDrawer(&physicsDebugDraw);
		//currentModel->DynamicsWorld.debugDrawWorld();
	}
}

void Gorc::Game::World::Level::LevelView::DrawThing(const Thing& thing) {
	if(!thing.Model3d || sector_vis_scratch.count(thing.Sector) == 0) {
		return;
	}

	float sector_light = currentModel->Sectors[thing.Sector].AmbientLight
			+ currentModel->Sectors[thing.Sector].ExtraLight;

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

				glActiveTexture(GL_TEXTURE0);
				material->Cels[0].Diffuse->Bind();

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				glActiveTexture(GL_TEXTURE1);
				material->Cels[0].Light->Bind();

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				glBegin(GL_TRIANGLES);

				Vector<3> first_geo = mesh.Vertices[std::get<0>(face.Vertices[0])];
				Vector<2> first_tex = mesh.TextureVertices[std::get<1>(face.Vertices[0])];
				float first_intensity = sector_light;

				for(size_t i = 2; i < face.Vertices.size(); ++i) {
					Vector<3> second_geo = mesh.Vertices[std::get<0>(face.Vertices[i - 1])];
					Vector<2> second_tex = mesh.TextureVertices[std::get<1>(face.Vertices[i - 1])];
					float second_intensity = sector_light;

					Vector<3> third_geo = mesh.Vertices[std::get<0>(face.Vertices[i])];
					Vector<2> third_tex = mesh.TextureVertices[std::get<1>(face.Vertices[i])];
					float third_intensity = sector_light;

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

void Gorc::Game::World::Level::LevelView::DrawSurface(unsigned int surf_num, const Content::Assets::LevelSector& sector) {
	const auto& surface = currentModel->Surfaces[surf_num];
	const auto& lev = currentModel->Level;

	if(surface.Material >= 0) {
		const auto& material_entry = currentModel->Level.Materials[surface.Material];
		const auto& material = std::get<0>(material_entry);

		float alpha = 1.0f;

		Vector<2> tex_scale = Vec(1.0f / static_cast<float>(material->Width),
				1.0f / static_cast<float>(material->Height));

		int surfaceCelNumber = surface.CelNumber;
		int actualSurfaceCelNumber;
		if(surfaceCelNumber >= 0) {
			actualSurfaceCelNumber = surfaceCelNumber % material->Cels.size();
		}
		else {
			actualSurfaceCelNumber = currentModel->MaterialCelNumber[surface.Material] % material->Cels.size();
		}

		glActiveTexture(GL_TEXTURE0);
		material->Cels[actualSurfaceCelNumber].Diffuse->Bind();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glActiveTexture(GL_TEXTURE1);
		material->Cels[actualSurfaceCelNumber].Light->Bind();

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
	}
}

void Gorc::Game::World::Level::LevelView::DrawSurfaceTranslucent(unsigned int surf_num, const Content::Assets::LevelSector& sector) {
	const auto& surface = currentModel->Surfaces[surf_num];
	const auto& lev = currentModel->Level;

	if(surface.Material >= 0) {
		const auto& material_entry = currentModel->Level.Materials[surface.Material];
		const auto& material = std::get<0>(material_entry);

		float alpha = (surface.FaceTypeFlags & Content::Assets::FaceTypeFlag::Translucent) ? 0.5f : 1.0f;

		Vector<2> tex_scale = Vec(1.0f / static_cast<float>(material->Width),
				1.0f / static_cast<float>(material->Height));

		int surfaceCelNumber = surface.CelNumber;
		int actualSurfaceCelNumber;
		if(surfaceCelNumber >= 0) {
			actualSurfaceCelNumber = surfaceCelNumber % material->Cels.size();
		}
		else {
			actualSurfaceCelNumber = currentModel->MaterialCelNumber[surface.Material] % material->Cels.size();
		}

		glActiveTexture(GL_TEXTURE0);
		material->Cels[actualSurfaceCelNumber].Diffuse->Bind();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glActiveTexture(GL_TEXTURE1);
		material->Cels[actualSurfaceCelNumber].Light->Bind();

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
	}
}

void Gorc::Game::World::Level::LevelView::DrawSector(unsigned int sec_num) {
	const Content::Assets::LevelSector& sector = currentModel->Sectors[sec_num];
	const Content::Assets::Level& lev = currentModel->Level;

	for(size_t i = sector.FirstSurface; i < sector.SurfaceCount + sector.FirstSurface; ++i) {
		const auto& surface = currentModel->Surfaces[i];

		if((surface.FaceTypeFlags & Content::Assets::FaceTypeFlag::Translucent) ||
				(surface.Adjoin >= 0 && surface.GeometryMode != Content::Assets::GeometryMode::NotDrawn)) {
			translucent_surfaces_scratch.push_back(std::make_tuple(sec_num, i, 0.0f));
		}
		else {
			DrawSurface(i, sector);
		}
	}
}

void Gorc::Game::World::Level::LevelView::DoSectorVis(unsigned int sec_num, const std::array<double, 16>& proj_mat, const std::array<double, 16>& view_mat,
		const std::array<int, 4>& viewport, const Math::Box<2, double>& adj_bbox, const Math::Vector<3>& cam_pos, const Math::Vector<3>& cam_look) {
	sector_visited_scratch.emplace(sec_num);

	const auto& sector = currentModel->Sectors[sec_num];
	for(unsigned int i = 0; i < sector.SurfaceCount; ++i) {
		const auto& surface = currentModel->Surfaces[sector.FirstSurface + i];

		const Math::Vector<3>& surf_vx_pos = currentModel->Level.Vertices[std::get<0>(surface.Vertices.front())];

		if(surface.Adjoin < 0 || sector_visited_scratch.count(surface.AdjoinedSector) > 0) {
			continue;
		}

		float dist = Math::Dot(surface.Normal, cam_pos - surf_vx_pos);
		if(dist < 0.0f) {
			continue;
		}

		double min_x = std::numeric_limits<double>::max();
		double min_y = std::numeric_limits<double>::max();
		double max_x = std::numeric_limits<double>::lowest();
		double max_y = std::numeric_limits<double>::lowest();

		bool failed = false;
		bool culled = true;
		for(const auto& vx : surface.Vertices) {
			auto vx_pos = currentModel->Level.Vertices[std::get<0>(vx)];

			if(Math::Dot(cam_look, vx_pos - cam_pos) < 0.0f) {
				// Vertex behind camera.
				failed = true;
				continue;
			}

			culled = false;

			double x_out, y_out, z_out;

			gluProject(Math::Get<0>(vx_pos), Math::Get<1>(vx_pos), Math::Get<2>(vx_pos),
					view_mat.data(), proj_mat.data(), viewport.data(),
					&x_out, &y_out, &z_out);

			min_x = std::min(min_x, x_out);
			max_x = std::max(max_x, x_out);
			min_y = std::min(min_y, y_out);
			max_y = std::max(max_y, y_out);
		}

		if(culled) {
			continue;
		}

		if(failed) {
			sector_vis_scratch.emplace(surface.AdjoinedSector);
			DoSectorVis(surface.AdjoinedSector, proj_mat, view_mat, viewport, adj_bbox, cam_pos, cam_look);
			continue;
		}

		Math::Box<2, double> new_adj_bbox(Math::Vec(min_x, min_y), Math::Vec(max_x, max_y));
		if(Math::BoxesOverlap(adj_bbox, new_adj_bbox)) {
			sector_vis_scratch.emplace(surface.AdjoinedSector);
			DoSectorVis(surface.AdjoinedSector, proj_mat, view_mat, viewport,
					Math::Intersect(adj_bbox, new_adj_bbox), cam_pos, cam_look);
		}
	}

	sector_visited_scratch.erase(sec_num);
}

void Gorc::Game::World::Level::LevelView::DrawLevel(const Math::Vector<3>& cam_pos) {
	translucent_surfaces_scratch.clear();

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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for(auto sec_num : sector_vis_scratch) {
		DrawSector(sec_num);
	}

	// Compute distances to translucent surfaces.
	for(auto& surf_tuple : translucent_surfaces_scratch) {
		unsigned int surf_id = std::get<1>(surf_tuple);
		const auto& surf = currentModel->Surfaces[surf_id];
		auto vx_pos = currentModel->Level.Vertices[std::get<0>(surf.Vertices.front())];
		std::get<2>(surf_tuple) = Math::Dot(surf.Normal, cam_pos - vx_pos);

		// TODO: Compute actual distance to polygon
	}

	std::sort(translucent_surfaces_scratch.begin(), translucent_surfaces_scratch.end(),
			[](const std::tuple<unsigned int, unsigned int, float>& a, const std::tuple<unsigned int, unsigned int, float>& b) {
		return std::get<2>(a) > std::get<2>(b);
	});
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
