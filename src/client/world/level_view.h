#pragma once

#include "framework/math/matrix.h"
#include "framework/view.h"
#include "framework/math/box.h"
#include "framework/utility/flag_set.h"
#include "content/flags/key_flag.h"
#include "level_shader.h"
#include "cog/verbs/table.h"
#include <stack>
#include <unordered_set>

namespace gorc {
namespace content {
class manager;

namespace assets {
class thing_template;
class level_sector;
class shader;
class model;
class animation;
class sprite;
class puppet;
}
}

namespace game {
class level_state;

namespace world {
class level_presenter;
class level_model;
class thing;
}

}

namespace client {

namespace world {

class level_view : public view {
private:
	surface_shader surfaceShader;
	horizon_shader horizonShader;
	ceiling_shader ceilingShader;
	light_shader lightShader;

	level_shader* currentLevelShader = nullptr;

	matrix<4> projection_matrix = make_identity_matrix<4>();
	matrix<4> view_matrix = make_identity_matrix<4>();
	std::stack<matrix<4>> model_matrix_stack;

	template <typename T, typename... U> void set_current_shader(T& shader, U... args) {
		currentLevelShader = &shader;
		shader.activate(args...);
		shader.set_projection_matrix(projection_matrix);
		shader.set_view_matrix(view_matrix);
		shader.set_model_matrix(model_matrix_stack.top());
	}

	game::world::level_presenter* currentPresenter = nullptr;
	game::world::level_model* currentModel = nullptr;
	std::unordered_set<unsigned int> sector_vis_scratch;
	std::unordered_set<unsigned int> sector_visited_scratch;
	std::vector<std::tuple<unsigned int, unsigned int>> horizon_sky_surfaces_scratch;
	std::vector<std::tuple<unsigned int, unsigned int>> ceiling_sky_surfaces_scratch;
	std::vector<std::tuple<unsigned int, unsigned int, float>> translucent_surfaces_scratch;
	std::vector<std::tuple<int, float>> visible_thing_scratch;

	void compute_visible_sectors(const box<2, int>& view_size);
	void record_visible_special_surfaces();
	void record_visible_things();
	void do_sector_vis(unsigned int sec_num, const std::array<double, 16>& proj_mat, const std::array<double, 16>& view_mat,
			const std::array<int, 4>& viewport, const box<2, double>& adj_bbox, const vector<3>& cam_pos, const vector<3>& cam_look);
	void draw_visible_diffuse_surfaces();
	void draw_visible_sky_surfaces(const box<2, int>& screen_size, const vector<3>& sector_tint);
	void draw_visible_translucent_surfaces_and_things();
	void draw_pov_model();

	void draw_surface(unsigned int surf_num, const content::assets::level_sector& sector, float alpha);
	void draw_sprite(const game::world::thing& thing, const content::assets::sprite& sprite, float sector_light);
	void draw_thing(const game::world::thing& thing, int thing_id);

public:
	level_view(content::manager& shadercontentmanager);

	inline void set_presenter(game::world::level_presenter* presenter) {
		currentPresenter = presenter;
	}

	inline void set_level_model(game::world::level_model* levelModel) {
		currentModel = levelModel;
	}

	inline void update_shader_model_matrix() {
		currentLevelShader->set_model_matrix(model_matrix_stack.top());
	}

	inline void concatenate_matrix(const matrix<4>& mat) {
		model_matrix_stack.top() = model_matrix_stack.top() * mat;
	}

	inline void push_matrix() {
		model_matrix_stack.push(model_matrix_stack.top());
	}

	inline void pop_matrix() {
		model_matrix_stack.pop();
	}

	virtual void draw(const time& time, const box<2, int>& view_size, graphics::render_target& target) override;

	class mesh_node_visitor {
	private:
		vector<4> sector_color;
		level_view& view;
		content::assets::model const* weapon_mesh;
		content::assets::puppet const* puppet_file;

	public:
		mesh_node_visitor(const vector<4>& sector_color, level_view& view, content::assets::model const* weapon_mesh,
				content::assets::puppet const* puppet_file);

		inline void concatenate_matrix(const matrix<4>& mat) {
			view.concatenate_matrix(mat);
			view.update_shader_model_matrix();
		}

		inline void push_matrix() {
			view.push_matrix();
		}

		inline void pop_matrix() {
			view.pop_matrix();
			view.update_shader_model_matrix();
		}

		void visit_mesh(const content::assets::model& model, int mesh_id, int node_id);
	};
};

}
}
}
