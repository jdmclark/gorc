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

Gorc::Game::World::Level::LevelView::LevelView(const Content::Assets::Shader& surfaceShader,
		const Content::Assets::Shader& horizonShader, const Content::Assets::Shader& ceilingShader)
	: surfaceShader(surfaceShader), horizonShader(horizonShader), ceilingShader(ceilingShader),
	  currentPresenter(nullptr), currentModel(nullptr) {
	return;
}

void Gorc::Game::World::Level::LevelView::Update(double dt) {
	if(currentPresenter) {
		currentPresenter->Update(dt);
	}
}

void Gorc::Game::World::Level::LevelView::SetUpViewProjectionMatrices(const Box<2, unsigned int>& view_size) {
	double aspect = static_cast<double>(view_size.Size<X>()) / static_cast<double>(view_size.Size<Y>());
	const auto& camera_thing = currentModel->Things[currentModel->CameraThingId];

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0, aspect, 0.001, 1000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	const Vector<3>& CamPosition = currentModel->CameraPosition;
	Vector<3> CamLookPoint = CamPosition + currentModel->CameraLook;
	gluLookAt(Get<X>(CamPosition), Get<Y>(CamPosition), Get<Z>(CamPosition),
			Get<X>(CamLookPoint), Get<Y>(CamLookPoint), Get<Z>(CamLookPoint),
			Get<X>(currentModel->CameraUp), Get<Y>(currentModel->CameraUp), Get<Z>(currentModel->CameraUp));
}

void Gorc::Game::World::Level::LevelView::ComputeVisibleSectors(const Box<2, unsigned int>& view_size) {
	unsigned int current_camera_sector_id = currentModel->Things[currentModel->CameraThingId].Sector;
	const auto& camera_pos = currentModel->CameraPosition;

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
	DoSectorVis(current_camera_sector_id, proj_matrix, view_matrix, viewport, adj_bbox, camera_pos, currentModel->CameraLook);
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

void Gorc::Game::World::Level::LevelView::ActivateSurfaceShader() {
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
	glUniform4fv(sector_tint_ul, 1, tint_color.data());
}

void Gorc::Game::World::Level::LevelView::ActivateHorizonShader(const Math::Box<2, unsigned int>& screen_size) {
	glUseProgram(horizonShader.program);

	unsigned int current_camera_sector_id = currentModel->Things[currentModel->CameraThingId].Sector;
	const auto& current_camera_sector = currentModel->Sectors[current_camera_sector_id];

	std::array<float, 4> tint_color;
	const auto& current_sector_tint = current_camera_sector.Tint;
	std::get<0>(tint_color) = Math::Get<0>(current_sector_tint);
	std::get<1>(tint_color) = Math::Get<1>(current_sector_tint);
	std::get<2>(tint_color) = Math::Get<2>(current_sector_tint);
	std::get<3>(tint_color) = Math::Length(current_sector_tint);

	std::array<float, 2> ssz { static_cast<float>(screen_size.Size<X>()), static_cast<float>(screen_size.Size<Y>()) };

	auto horizon_start_offset = currentModel->Header.HorizonSkyOffset / 256.0f;
	float horizons_per_rev = std::ceil(currentModel->Header.HorizonPixelsPerRev / 256.0f);
	const auto& camera_look = currentModel->CameraLook;
	float horizon_scale = 128.0f / currentModel->Header.HorizonDistance;

	float yaw_revs = std::atan2(Get<X>(camera_look), Get<Y>(camera_look)) / 6.28318f;
	Get<X>(horizon_start_offset) += yaw_revs * horizons_per_rev;

	float pitch_revs = std::acos(Dot(camera_look, Vec(0.0f, 0.0f, 1.0f))) * 0.5f + 1.0f;
	Get<Y>(horizon_start_offset) -= pitch_revs;

	std::array<float, 3> offset = { Get<0>(horizon_start_offset), Get<1>(horizon_start_offset), horizon_scale };

	auto diffuse_ul = glGetUniformLocation(horizonShader.program, "diffuse");
	glUniform1i(diffuse_ul, 0);
	auto light_ul = glGetUniformLocation(horizonShader.program, "light");
	glUniform1i(light_ul, 1);
	auto sector_tint_ul = glGetUniformLocation(horizonShader.program, "sector_tint");
	glUniform4fv(sector_tint_ul, 1, tint_color.data());
	auto ssz_ul = glGetUniformLocation(horizonShader.program, "screen_size");
	glUniform2fv(ssz_ul, 1, ssz.data());
	auto offset_ul = glGetUniformLocation(horizonShader.program, "horizon_sky_offset");
	glUniform3fv(offset_ul, 1, offset.data());
}

void Gorc::Game::World::Level::LevelView::ActivateCeilingShader() {
	glUseProgram(ceilingShader.program);

	unsigned int current_camera_sector_id = currentModel->Things[currentModel->CameraThingId].Sector;
	const auto& current_camera_sector = currentModel->Sectors[current_camera_sector_id];

	std::array<float, 4> tint_color;
	const auto& current_sector_tint = current_camera_sector.Tint;
	std::get<0>(tint_color) = Math::Get<0>(current_sector_tint);
	std::get<1>(tint_color) = Math::Get<1>(current_sector_tint);
	std::get<2>(tint_color) = Math::Get<2>(current_sector_tint);
	std::get<3>(tint_color) = Math::Length(current_sector_tint);

	std::array<float, 3> offset = { Get<0>(currentModel->Header.CeilingSkyOffset), Get<1>(currentModel->Header.CeilingSkyOffset),
			currentModel->Header.CeilingSkyZ };

	auto diffuse_ul = glGetUniformLocation(ceilingShader.program, "diffuse");
	glUniform1i(diffuse_ul, 0);
	auto light_ul = glGetUniformLocation(ceilingShader.program, "light");
	glUniform1i(light_ul, 1);
	auto sector_tint_ul = glGetUniformLocation(ceilingShader.program, "sector_tint");
	glUniform4fv(sector_tint_ul, 1, tint_color.data());
	auto offset_ul = glGetUniformLocation(ceilingShader.program, "ceiling_sky_offset");
	glUniform3fv(offset_ul, 1, offset.data());
}

void Gorc::Game::World::Level::LevelView::DrawVisibleDiffuseSurfaces() {
	const Content::Assets::Level& lev = currentModel->Level;

	for(auto sec_num : sector_vis_scratch) {
		const Content::Assets::LevelSector& sector = currentModel->Sectors[sec_num];

		for(size_t i = sector.FirstSurface; i < sector.FirstSurface + sector.SurfaceCount; ++i) {
			const auto& surface = currentModel->Surfaces[i];

			if(surface.Adjoin < 0 && (surface.Flags & Flags::SurfaceFlag::HorizonSky)) {
				horizon_sky_surfaces_scratch.push_back(std::make_tuple(sec_num, i));
			}
			else if(surface.Adjoin < 0 && (surface.Flags & Flags::SurfaceFlag::CeilingSky)) {
				ceiling_sky_surfaces_scratch.push_back(std::make_tuple(sec_num, i));
			}
			else if((surface.FaceTypeFlags & Flags::FaceFlag::Translucent) ||
					(surface.Adjoin >= 0 && surface.GeometryMode != Flags::GeometryMode::NotDrawn)) {
				translucent_surfaces_scratch.push_back(std::make_tuple(sec_num, i, 0.0f));
			}
			else {
				DrawSurface(i, sector, 1.0f);
			}
		}
	}
}

void Gorc::Game::World::Level::LevelView::DrawVisibleThings() {
	for(const auto& thing : currentModel->Things) {
		if((thing.Flags & Flags::ThingFlag::Invisible) || sector_vis_scratch.count(thing.Sector) == 0) {
			continue;
		}

		DrawThing(thing);
	}
}

void Gorc::Game::World::Level::LevelView::DrawVisibleTranslucentSurfaces() {
	const auto& cam_pos = currentModel->CameraPosition;

	// Compute distances to translucent surfaces
	for(auto& surf_tuple : translucent_surfaces_scratch) {
		unsigned int surf_id = std::get<1>(surf_tuple);
		const auto& surf = currentModel->Surfaces[surf_id];
		auto vx_pos = currentModel->Level.Vertices[std::get<0>(surf.Vertices.front())];
		std::get<2>(surf_tuple) = Math::Dot(surf.Normal, cam_pos - vx_pos);

		// TODO: Currently uses distance to plane. Compute actual distance to polygon.
	}

	// Sort translucent surfaces back to front.
	std::sort(translucent_surfaces_scratch.begin(), translucent_surfaces_scratch.end(),
			[](const std::tuple<unsigned int, unsigned int, float>& a, const std::tuple<unsigned int, unsigned int, float>& b) {
		return std::get<2>(a) > std::get<2>(b);
	});

	// Draw translucent surfaces
	for(auto& surf_tuple : translucent_surfaces_scratch) {
		DrawSurface(std::get<1>(surf_tuple), currentModel->Sectors[std::get<0>(surf_tuple)],
				(currentModel->Surfaces[std::get<1>(surf_tuple)].FaceTypeFlags & Flags::FaceFlag::Translucent) ? 0.5f : 1.0f);
	}
}

void Gorc::Game::World::Level::LevelView::DrawVisibleSkySurfaces(const Math::Box<2, unsigned int>& view_size) {
	if(!horizon_sky_surfaces_scratch.empty()) {
		ActivateHorizonShader(view_size);

		for(auto surf_tuple : horizon_sky_surfaces_scratch) {
			DrawSurface(std::get<1>(surf_tuple), currentModel->Sectors[std::get<0>(surf_tuple)], 1.0f);
		}
	}

	if(!ceiling_sky_surfaces_scratch.empty()) {
		ActivateCeilingShader();

		for(auto surf_tuple : ceiling_sky_surfaces_scratch) {
			DrawSurface(std::get<1>(surf_tuple), currentModel->Sectors[std::get<0>(surf_tuple)], 1.0f);
		}
	}
}

void Gorc::Game::World::Level::LevelView::Draw(double dt, const Math::Box<2, unsigned int>& view_size) {
	if(currentModel) {
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_ALPHA_TEST);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
		glShadeModel(GL_SMOOTH);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Clear surface scratch space
		horizon_sky_surfaces_scratch.clear();
		ceiling_sky_surfaces_scratch.clear();
		translucent_surfaces_scratch.clear();

		// Set up world and projection matrices
		SetUpViewProjectionMatrices(view_size);

		// Run level hidden surface removal
		ComputeVisibleSectors(view_size);

		// Prepare for rendering ordinary surfaces
		ActivateSurfaceShader();

		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);

		// Render ordinary surfaces and enqueue translucent/sky.
		DrawVisibleDiffuseSurfaces();

		// Render things
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		DrawVisibleThings();

		// TODO: Render skies
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		DrawVisibleSkySurfaces(view_size);

		// Render translucent surfaces
		ActivateSurfaceShader();
		glEnable(GL_BLEND);
		DrawVisibleTranslucentSurfaces();

		//currentModel->DynamicsWorld.setDebugDrawer(&physicsDebugDraw);
		//currentModel->DynamicsWorld.debugDrawWorld();
	}
}

void Gorc::Game::World::Level::LevelView::DrawSurface(unsigned int surf_num, const Content::Assets::LevelSector& sector, float alpha) {
	const auto& surface = currentModel->Surfaces[surf_num];
	const auto& lev = currentModel->Level;

	if(surface.Material >= 0) {
		const auto& material_entry = currentModel->Level.Materials[surface.Material];
		const auto& material = std::get<0>(material_entry);

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
		glBindTexture(GL_TEXTURE_2D, material->Cels[actualSurfaceCelNumber].Diffuse);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material->Cels[actualSurfaceCelNumber].Light);

		glBegin(GL_TRIANGLES);

		Vector<3> first_geo = lev.Vertices[std::get<0>(surface.Vertices[0])];
		Vector<2> first_tex = lev.TextureVertices[std::get<1>(surface.Vertices[0])] + surface.TextureOffset;
		float first_intensity = std::get<2>(surface.Vertices[0]) + sector.ExtraLight + surface.ExtraLight;

		for(size_t i = 2; i < surface.Vertices.size(); ++i) {
			Vector<3> second_geo = lev.Vertices[std::get<0>(surface.Vertices[i - 1])];
			Vector<2> second_tex = lev.TextureVertices[std::get<1>(surface.Vertices[i - 1])] + surface.TextureOffset;
			float second_intensity = std::get<2>(surface.Vertices[i - 1]) + sector.ExtraLight + surface.ExtraLight;

			Vector<3> third_geo = lev.Vertices[std::get<0>(surface.Vertices[i])];
			Vector<2> third_tex = lev.TextureVertices[std::get<1>(surface.Vertices[i])] + surface.TextureOffset;
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

void Gorc::Game::World::Level::LevelView::DrawMeshNode(const Content::Assets::Model& model, int mesh_id, float sector_light) {
	if(mesh_id < 0) {
		return;
	}

	const Content::Assets::ModelNode& node = model.HierarchyNodes[mesh_id];

	glPushMatrix();
	glTranslatef(Math::Get<0>(node.Offset), Math::Get<1>(node.Offset), Math::Get<2>(node.Offset));
	glRotatef(Math::Get<0>(node.Rotation), 1.0f, 0.0f, 0.0f);
	glRotatef(Math::Get<1>(node.Rotation), 0.0f, 0.0f, 1.0f);
	glRotatef(Math::Get<2>(node.Rotation), 0.0f, 1.0f, 0.0f);
	glTranslatef(Math::Get<0>(node.Pivot), Math::Get<1>(node.Pivot), Math::Get<2>(node.Pivot));

	if(node.Mesh >= 0) {
		const Content::Assets::ModelMesh& mesh = model.GeoSets.front().Meshes[node.Mesh];
		for(const auto& face : mesh.Faces) {
			if(face.Material >= 0) {
				const auto& material = model.Materials[face.Material];

				float alpha = (face.Type & Flags::FaceFlag::Translucent) ? 0.5f : 1.0f;

				Vector<2> tex_scale = Vec(1.0f / static_cast<float>(material->Width),
						1.0f / static_cast<float>(material->Height));

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, material->Cels[0].Diffuse);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, material->Cels[0].Light);

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

	glTranslatef(-Math::Get<0>(node.Pivot), -Math::Get<1>(node.Pivot), -Math::Get<2>(node.Pivot));

	DrawMeshNode(model, node.Child, sector_light);
	glPopMatrix();

	DrawMeshNode(model, node.Sibling, sector_light);
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
	DrawMeshNode(model, 0, sector_light);

	glPopMatrix();
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
