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

Gorc::Game::World::Level::LevelView::LevelView(Content::Manager& contentManager)
	: surfaceShader(contentManager.Load<Content::Assets::Shader>("surface.glsl")),
	  horizonShader(contentManager.Load<Content::Assets::Shader>("horizon.glsl")),
	  ceilingShader(contentManager.Load<Content::Assets::Shader>("ceiling.glsl")),
	  lightShader(contentManager.Load<Content::Assets::Shader>("light.glsl")),
	  currentPresenter(nullptr), currentModel(nullptr) {
	return;
}

void Gorc::Game::World::Level::LevelView::Update(double dt) {
	if(currentPresenter) {
		currentPresenter->Update(dt);
	}
}

void Gorc::Game::World::Level::LevelView::ComputeVisibleSectors(const Box<2, unsigned int>& view_size) {
	unsigned int current_camera_sector_id = currentModel->CameraSector;
	const auto& camera_pos = currentModel->CameraPosition;

	std::array<double, 16> proj_matrix;
	std::array<double, 16> view_matrix;
	std::array<int, 4> viewport;

	float const* proj = ProjectionMatrix.GetOpenGLMatrix();
	std::transform(proj, proj + 16, proj_matrix.begin(), [](float value) { return static_cast<double>(value); });

	float const* view = ViewMatrix.GetOpenGLMatrix();
	std::transform(view, view + 16, view_matrix.begin(), [](float value) { return static_cast<double>(value); });

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

void Gorc::Game::World::Level::LevelView::RecordVisibleSpecialSurfaces() {
	const Content::Assets::Level& lev = currentModel->Level;

	for(auto sec_num : sector_vis_scratch) {
		const Content::Assets::LevelSector& sector = currentModel->Sectors[sec_num];

		for(size_t i = sector.FirstSurface; i < sector.FirstSurface + sector.SurfaceCount; ++i) {
			const auto& surface = currentModel->Surfaces[i];

			if(surface.GeometryMode == Flags::GeometryMode::NotDrawn) {
				continue;
			}

			if(surface.Adjoin < 0 && (surface.Flags & Flags::SurfaceFlag::HorizonSky)) {
				horizon_sky_surfaces_scratch.push_back(std::make_tuple(sec_num, i));
			}
			else if(surface.Adjoin < 0 && (surface.Flags & Flags::SurfaceFlag::CeilingSky)) {
				ceiling_sky_surfaces_scratch.push_back(std::make_tuple(sec_num, i));
			}
			else if(surface.Adjoin >= 0) {
				translucent_surfaces_scratch.push_back(std::make_tuple(sec_num, i, 0.0f));
			}
		}
	}

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
}

void Gorc::Game::World::Level::LevelView::RecordVisibleThings() {
	for(auto& thing : currentModel->Things) {
		if(sector_vis_scratch.find(thing.Sector) != sector_vis_scratch.end()) {
			visible_thing_scratch.emplace_back(thing.GetId(), Math::Length(thing.Position - currentModel->CameraPosition));

			if(!(thing.Flags & Flags::ThingFlag::Sighted)) {
				// Thing has been sighted for first time. Fire sighted event.
				currentPresenter->ThingSighted(thing.GetId());
			}
		}
	}

	// Sort things from back to front.
	std::sort(visible_thing_scratch.begin(), visible_thing_scratch.end(),
			[](const std::tuple<int, float>& a, const std::tuple<int, float>& b) {
		return std::get<1>(a) > std::get<1>(b);
	});
}

void Gorc::Game::World::Level::LevelView::DrawVisibleDiffuseSurfaces() {
	glDepthMask(GL_TRUE);
	const Content::Assets::Level& lev = currentModel->Level;

	for(auto sec_num : sector_vis_scratch) {
		const Content::Assets::LevelSector& sector = currentModel->Sectors[sec_num];

		for(size_t i = sector.FirstSurface; i < sector.FirstSurface + sector.SurfaceCount; ++i) {
			const auto& surface = currentModel->Surfaces[i];

			if(surface.GeometryMode == Flags::GeometryMode::NotDrawn
					|| surface.Adjoin >= 0
					|| (surface.Flags & Flags::SurfaceFlag::HorizonSky)
					|| (surface.Flags & Flags::SurfaceFlag::CeilingSky)) {
				continue;
			}

			DrawSurface(i, sector, 1.0f);
		}
	}
}

void Gorc::Game::World::Level::LevelView::DrawVisibleSkySurfaces(const Math::Box<2, unsigned int>& view_size) {
	glDepthMask(GL_TRUE);

	if(!horizon_sky_surfaces_scratch.empty()) {
		SetCurrentShader(horizonShader, currentModel->Sectors[currentModel->CameraSector].Tint,
				currentModel->Header.HorizonSkyOffset, currentModel->Header.HorizonPixelsPerRev,
				currentModel->Header.HorizonDistance, view_size, currentModel->CameraLook);

		for(auto surf_tuple : horizon_sky_surfaces_scratch) {
			DrawSurface(std::get<1>(surf_tuple), currentModel->Sectors[std::get<0>(surf_tuple)], 1.0f);
		}
	}

	if(!ceiling_sky_surfaces_scratch.empty()) {
		SetCurrentShader(ceilingShader, currentModel->Sectors[currentModel->CameraSector].Tint,
				currentModel->Header.CeilingSkyOffset, currentModel->Header.CeilingSkyZ);

		for(auto surf_tuple : ceiling_sky_surfaces_scratch) {
			DrawSurface(std::get<1>(surf_tuple), currentModel->Sectors[std::get<0>(surf_tuple)], 1.0f);
		}
	}
}

void Gorc::Game::World::Level::LevelView::DrawVisibleTranslucentSurfacesAndThings() {
	auto thing_it = visible_thing_scratch.begin();
	auto surf_it = translucent_surfaces_scratch.begin();
	while(thing_it != visible_thing_scratch.end() && surf_it != translucent_surfaces_scratch.end()) {
		if(std::get<1>(*thing_it) <= std::get<2>(*surf_it)) {
			glDepthMask(GL_TRUE);
			glDisable(GL_CULL_FACE);
			DrawThing(currentModel->Things[std::get<0>(*thing_it)]);
			++thing_it;
		}
		else {
			glDepthMask(GL_FALSE);
			glEnable(GL_CULL_FACE);
			DrawSurface(std::get<1>(*surf_it), currentModel->Sectors[std::get<0>(*surf_it)],
					(currentModel->Surfaces[std::get<1>(*surf_it)].FaceTypeFlags & Flags::FaceFlag::Translucent) ? 0.5f : 1.0f);
			++surf_it;
		}
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);
	while(thing_it != visible_thing_scratch.end()) {
		DrawThing(currentModel->Things[std::get<0>(*thing_it)]);
		++thing_it;
	}

	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	while(surf_it != translucent_surfaces_scratch.end()) {
		DrawSurface(std::get<1>(*surf_it), currentModel->Sectors[std::get<0>(*surf_it)],
				(currentModel->Surfaces[std::get<1>(*surf_it)].FaceTypeFlags & Flags::FaceFlag::Translucent) ? 0.5f : 1.0f);
		++surf_it;
	}
}

void Gorc::Game::World::Level::LevelView::Draw(double dt, const Math::Box<2, unsigned int>& view_size) {
	if(currentModel) {
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_ALPHA_TEST);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
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
		visible_thing_scratch.clear();

		// Set up world and projection matrices
		double aspect = static_cast<double>(view_size.Size<X>()) / static_cast<double>(view_size.Size<Y>());

		ProjectionMatrix = Matrix<float>::MakePerspectiveMatrix(70.0f, static_cast<float>(aspect), 0.001f, 1000.0f);
		ViewMatrix = Matrix<float>::MakeLookMatrix(currentModel->CameraPosition, currentModel->CameraLook, currentModel->CameraUp);

		while(!ModelMatrixStack.empty()) {
			ModelMatrixStack.pop();
		}
		ModelMatrixStack.push(Matrix<float>::MakeIdentityMatrix());

		// Run level hidden surface removal
		ComputeVisibleSectors(view_size);
		RecordVisibleSpecialSurfaces();
		RecordVisibleThings();

		// Prepare for rendering ordinary surfaces
		SetCurrentShader(surfaceShader, currentModel->Sectors[currentModel->CameraSector].Tint);

		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);

		// Render ordinary surfaces and enqueue translucent/sky.
		DrawVisibleDiffuseSurfaces();

		// Render skies
		DrawVisibleSkySurfaces(view_size);

		// Interleave rendering things and translucent surfaces.
		glEnable(GL_BLEND);
		SetCurrentShader(surfaceShader, currentModel->Sectors[currentModel->CameraSector].Tint);

		DrawVisibleTranslucentSurfacesAndThings();

		// Draw lights
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_BLEND);
		for(const auto& light_thing : visible_thing_scratch) {
			const auto& thing = currentModel->Things[std::get<0>(light_thing)];
			if(thing.Light <= 0.0f) {
				continue;
			}

			SetCurrentShader(lightShader, thing.Position + thing.LightOffset, currentModel->CameraPosition, thing.LightIntensity, thing.Light);

			DrawVisibleDiffuseSurfaces();
			DrawVisibleTranslucentSurfacesAndThings();
		}

		glDepthMask(GL_TRUE);

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
			// TODO: Add MaterialAnim.
			actualSurfaceCelNumber = 0;
			//actualSurfaceCelNumber = currentModel->MaterialCelNumber[surface.Material] % material->Cels.size();
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

			glNormal3f(Get<X>(surface.Normal), Get<Y>(surface.Normal), Get<Z>(surface.Normal));
			glTexCoord2f(Get<X>(first_tex) * Get<X>(tex_scale), Get<Y>(first_tex) * Get<Y>(tex_scale));
			glColor4f(first_intensity, first_intensity, first_intensity, alpha);
			glVertex3f(Get<X>(first_geo), Get<Y>(first_geo), Get<Z>(first_geo));

			glNormal3f(Get<X>(surface.Normal), Get<Y>(surface.Normal), Get<Z>(surface.Normal));
			glTexCoord2f(Get<X>(second_tex) * Get<X>(tex_scale), Get<Y>(second_tex) * Get<Y>(tex_scale));
			glColor4f(second_intensity, second_intensity, second_intensity, alpha);
			glVertex3f(Get<X>(second_geo), Get<Y>(second_geo), Get<Z>(second_geo));

			glNormal3f(Get<X>(surface.Normal), Get<Y>(surface.Normal), Get<Z>(surface.Normal));
			glTexCoord2f(Get<X>(third_tex) * Get<X>(tex_scale), Get<Y>(third_tex) * Get<Y>(tex_scale));
			glColor4f(third_intensity, third_intensity, third_intensity, alpha);
			glVertex3f(Get<X>(third_geo), Get<Y>(third_geo), Get<Z>(third_geo));
		}

		glEnd();
	}
}

void Gorc::Game::World::Level::LevelView::DrawMeshNode(const Thing& thing, const Content::Assets::Model& model,
		int mesh_id, float sector_light) {
	if(mesh_id < 0) {
		return;
	}

	const Content::Assets::ModelNode& node = model.HierarchyNodes[mesh_id];

	PushMatrix();

	Math::Vector<3> anim_translate = Math::Zero<3>();
	Math::Vector<3> anim_rotate = Math::Zero<3>();

	if(thing.AttachedKeyMix >= 0) {
		std::tie(anim_translate, anim_rotate) = currentPresenter->KeyPresenter.GetNodeFrame(thing.AttachedKeyMix, mesh_id, node.Type);
	}
	else {
		anim_translate += node.Offset;
		anim_rotate += node.Rotation;
	}

	ConcatenateMatrix(Math::Matrix<float>::MakeTranslationMatrix(anim_translate));
	ConcatenateMatrix(Math::Matrix<float>::MakeRotationMatrix(Math::Get<1>(anim_rotate), Math::Vec(0.0f, 0.0f, 1.0f)));
	ConcatenateMatrix(Math::Matrix<float>::MakeRotationMatrix(Math::Get<0>(anim_rotate), Math::Vec(1.0f, 0.0f, 0.0f)));
	ConcatenateMatrix(Math::Matrix<float>::MakeRotationMatrix(Math::Get<2>(anim_rotate), Math::Vec(0.0f, 1.0f, 0.0f)));
	ConcatenateMatrix(Math::Matrix<float>::MakeTranslationMatrix(node.Pivot));
	UpdateShaderModelMatrix();

	if(node.Mesh >= 0) {
		const Content::Assets::ModelMesh& mesh = model.GeoSets.front().Meshes[node.Mesh];
		for(const auto& face : mesh.Faces) {
			if(face.Material >= 0) {
				const auto& material = model.Materials[face.Material];

				float alpha = (face.Type & Flags::FaceFlag::Translucent) ? 0.5f : 1.0f;

				Vector<2> tex_scale = Vec(1.0f / static_cast<float>(material->Width),
						1.0f / static_cast<float>(material->Height));

				float light = sector_light + face.ExtraLight;
				if(face.Light == Flags::LightMode::FullyLit || mesh.Light == Flags::LightMode::FullyLit) {
					light = 1.0f;
				}

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, material->Cels[0].Diffuse);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, material->Cels[0].Light);

				glBegin(GL_TRIANGLES);

				Vector<3> first_geo = mesh.Vertices[std::get<0>(face.Vertices[0])];
				Vector<2> first_tex = mesh.TextureVertices[std::get<1>(face.Vertices[0])];
				Vector<3> first_normal = mesh.VertexNormals[std::get<0>(face.Vertices[0])];
				float first_intensity = light;

				for(size_t i = 2; i < face.Vertices.size(); ++i) {
					Vector<3> second_geo = mesh.Vertices[std::get<0>(face.Vertices[i - 1])];
					Vector<2> second_tex = mesh.TextureVertices[std::get<1>(face.Vertices[i - 1])];
					Vector<3> second_normal = mesh.VertexNormals[std::get<0>(face.Vertices[i - 1])];
					float second_intensity = light;

					Vector<3> third_geo = mesh.Vertices[std::get<0>(face.Vertices[i])];
					Vector<2> third_tex = mesh.TextureVertices[std::get<1>(face.Vertices[i])];
					Vector<3> third_normal = mesh.VertexNormals[std::get<0>(face.Vertices[i])];
					float third_intensity = light;

					glNormal3f(Get<X>(first_normal), Get<Y>(first_normal), Get<Z>(first_normal));
					glTexCoord2f(Get<X>(first_tex) * Get<X>(tex_scale), Get<Y>(first_tex) * Get<Y>(tex_scale));
					glColor4f(first_intensity, first_intensity, first_intensity, alpha);
					glVertex3f(Get<X>(first_geo), Get<Y>(first_geo), Get<Z>(first_geo));

					glNormal3f(Get<X>(second_normal), Get<Y>(second_normal), Get<Z>(second_normal));
					glTexCoord2f(Get<X>(second_tex) * Get<X>(tex_scale), Get<Y>(second_tex) * Get<Y>(tex_scale));
					glColor4f(second_intensity, second_intensity, second_intensity, alpha);
					glVertex3f(Get<X>(second_geo), Get<Y>(second_geo), Get<Z>(second_geo));

					glNormal3f(Get<X>(third_normal), Get<Y>(third_normal), Get<Z>(third_normal));
					glTexCoord2f(Get<X>(third_tex) * Get<X>(tex_scale), Get<Y>(third_tex) * Get<Y>(tex_scale));
					glColor4f(third_intensity, third_intensity, third_intensity, alpha);
					glVertex3f(Get<X>(third_geo), Get<Y>(third_geo), Get<Z>(third_geo));
				}

				glEnd();
			}
		}
	}

	ConcatenateMatrix(Math::Matrix<float>::MakeTranslationMatrix(-node.Pivot));

	DrawMeshNode(thing, model, node.Child, sector_light);
	PopMatrix();
	UpdateShaderModelMatrix();

	DrawMeshNode(thing, model, node.Sibling, sector_light);
}

void Gorc::Game::World::Level::LevelView::DrawThing(const Thing& thing) {
	if(!thing.Model3d || (thing.Flags & Flags::ThingFlag::Invisible)) {
		return;
	}

	float sector_light = currentModel->Sectors[thing.Sector].AmbientLight
			+ currentModel->Sectors[thing.Sector].ExtraLight;

	PushMatrix();

	if(thing.RigidBody) {
		btTransform trns;
		thing.RigidBody->getMotionState()->getWorldTransform(trns);

		Math::Matrix<float> mat;
		trns.getOpenGLMatrix(mat.GetOpenGLMatrix());

		ConcatenateMatrix(mat);
	}
	else {
		// Fall back to p/y/r orientation when physics is not used.
		ConcatenateMatrix(Math::Matrix<float>::MakeTranslationMatrix(thing.Position));
		ConcatenateMatrix(Math::Matrix<float>::MakeRotationMatrix(Math::Get<1>(thing.Orientation), Math::Vec(0.0f, 0.0f, 1.0f)));
		ConcatenateMatrix(Math::Matrix<float>::MakeRotationMatrix(Math::Get<0>(thing.Orientation), Math::Vec(1.0f, 0.0f, 0.0f)));
		ConcatenateMatrix(Math::Matrix<float>::MakeRotationMatrix(Math::Get<2>(thing.Orientation), Math::Vec(0.0f, 1.0f, 0.0f)));
	}

	UpdateShaderModelMatrix();

	const Content::Assets::Model& model = *thing.Model3d;
	DrawMeshNode(thing, model, 0, sector_light);

	PopMatrix();
	UpdateShaderModelMatrix();
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
