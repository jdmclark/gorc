#include "content/assets/shader.h"
#include "level_view.h"
#include "level_presenter.h"
#include "game/components.h"
#include "level_model.h"
#include "content/assets/model.h"
#include "content/assets/sprite.h"
#include "content/constants.h"

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <GL/glu.h>

using namespace gorc::math;

gorc::game::world::level::level_view::level_view(content::manager& contentmanager)
	: surfaceShader(contentmanager.load<content::assets::shader>("surface.glsl")),
	  horizonShader(contentmanager.load<content::assets::shader>("horizon.glsl")),
	  ceilingShader(contentmanager.load<content::assets::shader>("ceiling.glsl")),
	  lightShader(contentmanager.load<content::assets::shader>("light.glsl")),
	  currentPresenter(nullptr), currentModel(nullptr) {
	return;
}

void gorc::game::world::level::level_view::update(double dt) {
	if(currentPresenter) {
		currentPresenter->update(dt);
	}
}

void gorc::game::world::level::level_view::compute_visible_sectors(const box<2, int>& view_size) {
	unsigned int current_camera_sector_id = currentModel->camera_sector;
	const auto& camera_pos = currentModel->camera_position;

	std::array<double, 16> proj_matrix;
	std::array<double, 16> view_matrix;
	std::array<int, 4> viewport;

	float const* proj = make_opengl_matrix(projection_matrix);
	std::transform(proj, proj + 16, proj_matrix.begin(), [](float value) { return static_cast<double>(value); });

	float const* view = make_opengl_matrix(this->view_matrix);
	std::transform(view, view + 16, view_matrix.begin(), [](float value) { return static_cast<double>(value); });

	glGetIntegerv(GL_VIEWPORT, viewport.data());

	box<2, double> adj_bbox(make_vector<double>(static_cast<double>(std::get<0>(viewport)), static_cast<double>(std::get<1>(viewport))),
			make_vector<double>(static_cast<double>(std::get<0>(viewport) + std::get<2>(viewport)),
					static_cast<double>(std::get<1>(viewport) + std::get<3>(viewport))));

	sector_visited_scratch.clear();
	sector_vis_scratch.clear();
	sector_vis_scratch.emplace(current_camera_sector_id);
	do_sector_vis(current_camera_sector_id, proj_matrix, view_matrix, viewport, adj_bbox, camera_pos, currentModel->camera_look);
}

void gorc::game::world::level::level_view::do_sector_vis(unsigned int sec_num, const std::array<double, 16>& proj_mat, const std::array<double, 16>& view_mat,
		const std::array<int, 4>& viewport, const box<2, double>& adj_bbox, const vector<3>& cam_pos, const vector<3>& cam_look) {
	sector_visited_scratch.emplace(sec_num);

	const auto& sector = currentModel->sectors[sec_num];
	for(unsigned int i = 0; i < sector.surface_count; ++i) {
		const auto& surface = currentModel->surfaces[sector.first_surface + i];

		const vector<3>& surf_vx_pos = currentModel->level.vertices[std::get<0>(surface.vertices.front())];

		if(surface.adjoin < 0 || sector_visited_scratch.count(surface.adjoined_sector) > 0) {
			continue;
		}

		float dist = dot(surface.normal, cam_pos - surf_vx_pos);
		if(dist < 0.0f) {
			continue;
		}

		double min_x = std::numeric_limits<double>::max();
		double min_y = std::numeric_limits<double>::max();
		double max_x = std::numeric_limits<double>::lowest();
		double max_y = std::numeric_limits<double>::lowest();

		bool failed = false;
		bool culled = true;
		for(const auto& vx : surface.vertices) {
			auto vx_pos = currentModel->level.vertices[std::get<0>(vx)];

			if(dot(cam_look, vx_pos - cam_pos) < 0.0f) {
				// Vertex behind camera.
				failed = true;
				continue;
			}

			culled = false;

			double x_out, y_out, z_out;

			gluProject(get<0>(vx_pos), get<1>(vx_pos), get<2>(vx_pos),
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
			sector_vis_scratch.emplace(surface.adjoined_sector);
			do_sector_vis(surface.adjoined_sector, proj_mat, view_mat, viewport, adj_bbox, cam_pos, cam_look);
			continue;
		}

		box<2, double> new_adj_bbox(make_vector(min_x, min_y), make_vector(max_x, max_y));
		if(adj_bbox.overlaps(new_adj_bbox)) {
			sector_vis_scratch.emplace(surface.adjoined_sector);
			do_sector_vis(surface.adjoined_sector, proj_mat, view_mat, viewport,
					adj_bbox & new_adj_bbox, cam_pos, cam_look);
		}
	}

	sector_visited_scratch.erase(sec_num);
}

void gorc::game::world::level::level_view::record_visible_special_surfaces() {
	const content::assets::level& lev = currentModel->level;

	for(auto sec_num : sector_vis_scratch) {
		const content::assets::level_sector& sector = currentModel->sectors[sec_num];

		for(size_t i = sector.first_surface; i < sector.first_surface + sector.surface_count; ++i) {
			const auto& surface = currentModel->surfaces[i];

			if(surface.geometry_mode == flags::geometry_mode::NotDrawn) {
				continue;
			}

			if(surface.adjoin < 0 && (surface.flags & flags::surface_flag::HorizonSky)) {
				horizon_sky_surfaces_scratch.push_back(std::make_tuple(sec_num, i));
			}
			else if(surface.adjoin < 0 && (surface.flags & flags::surface_flag::CeilingSky)) {
				ceiling_sky_surfaces_scratch.push_back(std::make_tuple(sec_num, i));
			}
			else if(surface.adjoin >= 0) {
				translucent_surfaces_scratch.push_back(std::make_tuple(sec_num, i, 0.0f));
			}
		}
	}

	const auto& cam_pos = currentModel->camera_position;

	// Compute distances to translucent surfaces
	for(auto& surf_tuple : translucent_surfaces_scratch) {
		unsigned int surf_id = std::get<1>(surf_tuple);
		const auto& surf = currentModel->surfaces[surf_id];
		auto vx_pos = currentModel->level.vertices[std::get<0>(surf.vertices.front())];
		std::get<2>(surf_tuple) = dot(surf.normal, cam_pos - vx_pos);

		// TODO: Currently uses distance to plane. Compute actual distance to polygon.
	}

	// Sort translucent surfaces back to front.
	std::sort(translucent_surfaces_scratch.begin(), translucent_surfaces_scratch.end(),
			[](const std::tuple<unsigned int, unsigned int, float>& a, const std::tuple<unsigned int, unsigned int, float>& b) {
		return std::get<2>(a) > std::get<2>(b);
	});
}

void gorc::game::world::level::level_view::record_visible_things() {
	for(auto& thing : currentModel->things) {
		if(sector_vis_scratch.find(thing.sector) != sector_vis_scratch.end()) {
			visible_thing_scratch.emplace_back(thing.get_id(), length(thing.position - currentModel->camera_position));

			if(!(thing.flags & flags::thing_flag::Sighted)) {
				// thing has been sighted for first time. Fire sighted event.
				currentPresenter->thing_sighted(thing.get_id());
			}
		}
	}

	// Sort things from back to front.
	std::sort(visible_thing_scratch.begin(), visible_thing_scratch.end(),
			[](const std::tuple<int, float>& a, const std::tuple<int, float>& b) {
		return std::get<1>(a) > std::get<1>(b);
	});
}

void gorc::game::world::level::level_view::draw_visible_diffuse_surfaces() {
	glDepthMask(GL_TRUE);
	const content::assets::level& lev = currentModel->level;

	for(auto sec_num : sector_vis_scratch) {
		const content::assets::level_sector& sector = currentModel->sectors[sec_num];

		for(size_t i = sector.first_surface; i < sector.first_surface + sector.surface_count; ++i) {
			const auto& surface = currentModel->surfaces[i];

			if(surface.geometry_mode == flags::geometry_mode::NotDrawn
					|| surface.adjoin >= 0
					|| (surface.flags & flags::surface_flag::HorizonSky)
					|| (surface.flags & flags::surface_flag::CeilingSky)) {
				continue;
			}

			draw_surface(i, sector, 1.0f);
		}
	}
}

void gorc::game::world::level::level_view::draw_visible_sky_surfaces(const box<2, int>& view_size, const vector<3>& sector_tint) {
	glDepthMask(GL_TRUE);

	if(!horizon_sky_surfaces_scratch.empty()) {
		set_current_shader(horizonShader, sector_tint,
				currentModel->header.horizon_sky_offset, currentModel->header.horizon_pixels_per_rev,
				currentModel->header.horizon_distance, view_size, currentModel->camera_look);

		for(auto surf_tuple : horizon_sky_surfaces_scratch) {
			draw_surface(std::get<1>(surf_tuple), currentModel->sectors[std::get<0>(surf_tuple)], 1.0f);
		}
	}

	if(!ceiling_sky_surfaces_scratch.empty()) {
		set_current_shader(ceilingShader, sector_tint,
				currentModel->header.ceiling_sky_offset, currentModel->header.ceiling_sky_z);

		for(auto surf_tuple : ceiling_sky_surfaces_scratch) {
			draw_surface(std::get<1>(surf_tuple), currentModel->sectors[std::get<0>(surf_tuple)], 1.0f);
		}
	}
}

void gorc::game::world::level::level_view::draw_visible_translucent_surfaces_and_things() {
	auto thing_it = visible_thing_scratch.begin();
	auto surf_it = translucent_surfaces_scratch.begin();
	while(thing_it != visible_thing_scratch.end() && surf_it != translucent_surfaces_scratch.end()) {
		if(std::get<1>(*thing_it) <= std::get<2>(*surf_it)) {
			glDepthMask(GL_TRUE);
			glDisable(GL_CULL_FACE);
			draw_thing(currentModel->things[std::get<0>(*thing_it)]);
			++thing_it;
		}
		else {
			glDepthMask(GL_FALSE);
			glEnable(GL_CULL_FACE);
			draw_surface(std::get<1>(*surf_it), currentModel->sectors[std::get<0>(*surf_it)],
					(currentModel->surfaces[std::get<1>(*surf_it)].face_type_flags & flags::face_flag::Translucent) ? 0.5f : 1.0f);
			++surf_it;
		}
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);
	while(thing_it != visible_thing_scratch.end()) {
		draw_thing(currentModel->things[std::get<0>(*thing_it)]);
		++thing_it;
	}

	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	while(surf_it != translucent_surfaces_scratch.end()) {
		draw_surface(std::get<1>(*surf_it), currentModel->sectors[std::get<0>(*surf_it)],
				(currentModel->surfaces[std::get<1>(*surf_it)].face_type_flags & flags::face_flag::Translucent) ? 0.5f : 1.0f);
		++surf_it;
	}
}

void gorc::game::world::level::level_view::draw(double dt, const box<2, int>& view_size) {
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
		double aspect = static_cast<double>(get_size<0>(view_size)) / static_cast<double>(get_size<1>(view_size));

		projection_matrix = make_perspective_matrix(70.0f, static_cast<float>(aspect), 0.001f, 1000.0f);
		view_matrix = make_look_matrix(currentModel->camera_position, currentModel->camera_look, currentModel->camera_up);

		while(!model_matrix_stack.empty()) {
			model_matrix_stack.pop();
		}
		model_matrix_stack.push(make_identity_matrix<4, float>());

		// Run level hidden surface removal
		compute_visible_sectors(view_size);
		record_visible_special_surfaces();
		record_visible_things();

		// Prepare for rendering ordinary surfaces
		auto sector_tint = currentModel->sectors[currentModel->camera_sector].tint;
		sector_tint = (sector_tint * length(sector_tint)) + (currentModel->dynamic_tint * (1.0f - length(sector_tint)));

		set_current_shader(surfaceShader, sector_tint);

		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);

		// Render ordinary surfaces and enqueue translucent/sky.
		draw_visible_diffuse_surfaces();

		// Render skies
		draw_visible_sky_surfaces(view_size, sector_tint);

		// Interleave rendering things and translucent surfaces.
		glEnable(GL_BLEND);
		set_current_shader(surfaceShader, sector_tint);

		draw_visible_translucent_surfaces_and_things();

		// Draw lights
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		for(const auto& light_thing : visible_thing_scratch) {
			const auto& thing = currentModel->things[std::get<0>(light_thing)];

			float light = thing.light + ((thing.actor_flags & flags::actor_flag::HasFieldlight) ? thing.light_intensity : 0.0f);

			if(light <= 0.0f) {
				continue;
			}

			set_current_shader(lightShader, thing.position + thing.light_offset, currentModel->camera_position, light, light);

			draw_visible_diffuse_surfaces();
			draw_visible_translucent_surfaces_and_things();
		}

		glDepthMask(GL_TRUE);

		glDisable(GL_DEPTH_TEST);
	}
}

void gorc::game::world::level::level_view::draw_surface(unsigned int surf_num, const content::assets::level_sector& sector, float alpha) {
	const auto& surface = currentModel->surfaces[surf_num];
	const auto& lev = currentModel->level;

	if(surface.material >= 0) {
		const auto& material_entry = currentModel->level.materials[surface.material];
		const auto& material = std::get<0>(material_entry);

		vector<2> tex_scale = make_vector(1.0f / static_cast<float>(material->width),
				1.0f / static_cast<float>(material->height));

		int surfaceCelNumber = surface.cel_number;
		int actualSurfaceCelNumber;
		if(surfaceCelNumber >= 0) {
			actualSurfaceCelNumber = surfaceCelNumber % material->cels.size();
		}
		else {
			// TODO: Add MaterialAnim.
			actualSurfaceCelNumber = 0;
			//actualSurfaceCelNumber = currentModel->MaterialCelNumber[surface.material] % material->Cels.size();
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material->cels[actualSurfaceCelNumber].diffuse);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material->cels[actualSurfaceCelNumber].light);

		glBegin(GL_TRIANGLES);

		vector<3> first_geo = lev.vertices[std::get<0>(surface.vertices[0])];
		vector<2> first_tex = lev.texture_vertices[std::get<1>(surface.vertices[0])] + surface.texture_offset;
		float first_intensity = std::get<2>(surface.vertices[0]) + sector.extra_light + surface.extra_light;

		for(size_t i = 2; i < surface.vertices.size(); ++i) {
			vector<3> second_geo = lev.vertices[std::get<0>(surface.vertices[i - 1])];
			vector<2> second_tex = lev.texture_vertices[std::get<1>(surface.vertices[i - 1])] + surface.texture_offset;
			float second_intensity = std::get<2>(surface.vertices[i - 1]) + sector.extra_light + surface.extra_light;

			vector<3> third_geo = lev.vertices[std::get<0>(surface.vertices[i])];
			vector<2> third_tex = lev.texture_vertices[std::get<1>(surface.vertices[i])] + surface.texture_offset;
			float third_intensity = std::get<2>(surface.vertices[i]) + sector.extra_light + surface.extra_light;

			apply(glNormal3f, surface.normal);
			glTexCoord2f(get<0>(first_tex) * get<0>(tex_scale), get<1>(first_tex) * get<1>(tex_scale));
			glColor4f(first_intensity, first_intensity, first_intensity, alpha);
			apply(glVertex3f, first_geo);

			apply(glNormal3f, surface.normal);
			glTexCoord2f(get<0>(second_tex) * get<0>(tex_scale), get<1>(second_tex) * get<1>(tex_scale));
			glColor4f(second_intensity, second_intensity, second_intensity, alpha);
			apply(glVertex3f, second_geo);

			apply(glNormal3f, surface.normal);
			glTexCoord2f(get<0>(third_tex) * get<0>(tex_scale), get<1>(third_tex) * get<1>(tex_scale));
			glColor4f(third_intensity, third_intensity, third_intensity, alpha);
			apply(glVertex3f, third_geo);
		}

		glEnd();
	}
}

gorc::game::world::level::level_view::mesh_node_visitor::mesh_node_visitor(float sector_light, level_view& view)
	: sector_light(sector_light), view(view) {
	return;
}

void gorc::game::world::level::level_view::mesh_node_visitor::visit_mesh(const content::assets::model& model, int mesh_id) {
	const content::assets::model_mesh& mesh = model.geosets.front().meshes[mesh_id];
	for(const auto& face : mesh.faces) {
		if(face.material >= 0) {
			const auto& material = model.materials[face.material];

			float alpha = (face.type & flags::face_flag::Translucent) ? 0.5f : 1.0f;

			vector<2> tex_scale = make_vector(1.0f / static_cast<float>(material->width),
					1.0f / static_cast<float>(material->height));

			float light = sector_light + face.extra_light;
			if(face.light == flags::light_mode::FullyLit || mesh.light == flags::light_mode::FullyLit) {
				light = 1.0f;
			}

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, material->cels[0].diffuse);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, material->cels[0].light);

			glBegin(GL_TRIANGLES);

			vector<3> first_geo = mesh.vertices[std::get<0>(face.vertices[0])];
			vector<2> first_tex = mesh.texture_vertices[std::get<1>(face.vertices[0])];
			vector<3> first_normal = mesh.vertex_normals[std::get<0>(face.vertices[0])];
			float first_intensity = light;

			for(size_t i = 2; i < face.vertices.size(); ++i) {
				vector<3> second_geo = mesh.vertices[std::get<0>(face.vertices[i - 1])];
				vector<2> second_tex = mesh.texture_vertices[std::get<1>(face.vertices[i - 1])];
				vector<3> second_normal = mesh.vertex_normals[std::get<0>(face.vertices[i - 1])];
				float second_intensity = light;

				vector<3> third_geo = mesh.vertices[std::get<0>(face.vertices[i])];
				vector<2> third_tex = mesh.texture_vertices[std::get<1>(face.vertices[i])];
				vector<3> third_normal = mesh.vertex_normals[std::get<0>(face.vertices[i])];
				float third_intensity = light;

				apply(glNormal3f, first_normal);
				glTexCoord2f(get<0>(first_tex) * get<0>(tex_scale), get<1>(first_tex) * get<1>(tex_scale));
				glColor4f(first_intensity, first_intensity, first_intensity, alpha);
				apply(glVertex3f, first_geo);

				apply(glNormal3f, second_normal);
				glTexCoord2f(get<0>(second_tex) * get<0>(tex_scale), get<1>(second_tex) * get<1>(tex_scale));
				glColor4f(second_intensity, second_intensity, second_intensity, alpha);
				apply(glVertex3f, second_geo);

				apply(glNormal3f, third_normal);
				glTexCoord2f(get<0>(third_tex) * get<0>(tex_scale), get<1>(third_tex) * get<1>(tex_scale));
				glColor4f(third_intensity, third_intensity, third_intensity, alpha);
				apply(glVertex3f, third_geo);
			}

			glEnd();
		}
	}
}

void gorc::game::world::level::level_view::draw_sprite(const thing& thing, const content::assets::sprite& sprite, float sector_light) {
	if(sprite.material) {
		float light = sector_light + sprite.extra_light;
		if(sprite.light_mode == flags::light_mode::FullyLit) {
			light = 1.0f;
		}

		vector<3> offset = cross(currentModel->camera_look, currentModel->camera_up) * get<0>(sprite.offset) +
				currentModel->camera_look * get<1>(sprite.offset) +
				currentModel->camera_up * get<2>(sprite.offset);
		concatenate_matrix(make_translation_matrix(thing.position + offset));

		matrix<4> new_model = view_matrix.transpose();
		get<3, 0>(new_model) = 0.0f;
		get<3, 1>(new_model) = 0.0f;
		get<3, 2>(new_model) = 0.0f;
		get<3, 3>(new_model) = 1.0f;
		concatenate_matrix(new_model);

		update_shader_model_matrix();

		// TODO: Currently plays animation over duration of timer. Behavior should be verified.
		int current_frame = 0;
		if(thing.timer) {
			current_frame = static_cast<int>(std::floor(sprite.material->cels.size() * thing.time_alive / thing.timer)) % sprite.material->cels.size();
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sprite.material->cels[current_frame].diffuse);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, sprite.material->cels[current_frame].light);

		vector<3> sprite_middle = make_zero_vector<3, float>();//sprite.Offset;
		vector<3> horiz_off = make_vector(1.0f,0.0f,0.0f) * sprite.width * 0.5f;
		vector<3> vert_off = make_vector(0.0f,1.0f,0.0f) * sprite.height * 0.5f;

		vector<3> sprite_vx1 = sprite_middle + horiz_off + vert_off;
		vector<3> sprite_vx2 = sprite_middle + horiz_off - vert_off;
		vector<3> sprite_vx3 = sprite_middle - horiz_off - vert_off;
		vector<3> sprite_vx4 = sprite_middle - horiz_off + vert_off;

		vector<3> sprite_normal = make_vector(0.0f, 1.0f, 0.0f);

		auto light_vec = make_vector(light, light, light, 1.0f);

		glDepthMask(GL_FALSE);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);
		glBegin(GL_QUADS);

		apply(glNormal3f, sprite_normal);
		glTexCoord2f(1,0);
		apply(glColor4f, light_vec);
		apply(glVertex3f, sprite_vx1);

		apply(glNormal3f, sprite_normal);
		glTexCoord2f(1,1);
		apply(glColor4f, light_vec);
		apply(glVertex3f, sprite_vx2);

		apply(glNormal3f, sprite_normal);
		glTexCoord2f(0,1);
		apply(glColor4f, light_vec);
		apply(glVertex3f, sprite_vx3);

		apply(glNormal3f, sprite_normal);
		glTexCoord2f(0,0);
		apply(glColor4f, light_vec);
		apply(glVertex3f, sprite_vx4);

		glEnd();
		glDepthMask(GL_TRUE);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
}

void gorc::game::world::level::level_view::draw_thing(const thing& thing) {
	if(thing.flags & flags::thing_flag::Invisible) {
		return;
	}

	float sector_light = currentModel->sectors[thing.sector].ambient_light
			+ currentModel->sectors[thing.sector].extra_light;

	if(thing.model_3d) {
		mesh_node_visitor v(sector_light, *this);
		currentPresenter->key_presenter.visit_mesh_hierarchy(v, thing);
	}
	else if(thing.sprite) {
		push_matrix();
		draw_sprite(thing, *thing.sprite, sector_light);
		pop_matrix();
		update_shader_model_matrix();
	}
}
