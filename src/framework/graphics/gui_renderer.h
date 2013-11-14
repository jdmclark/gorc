#pragma once

#include "framework/content/assets/texture.h"
#include "framework/content/assets/shader.h"
#include "framework/math/box.h"
#include "framework/math/matrix.h"
#include <tuple>
#include <map>

namespace gorc {
namespace graphics {

class gui_renderer {
private:
	std::multimap<std::tuple<int, content::assets::texture const*>, std::tuple<box<2, int>, box<2, float>>> sprites;

	class gui_shader {
	private:
		const GLuint program;
		const GLint model_mat_ul, view_mat_ul, proj_mat_ul;
		const GLint diffuse_ul;

	public:
		gui_shader(const content::assets::shader& shader);
		void set_view_matrix(const matrix<4, float>& view_matrix) const;
		void set_projection_matrix(const matrix<4, float>& proj_matrix) const;
		void set_model_matrix(const matrix<4, float>& model_matrix) const;
		void activate() const;
	} shader;

public:
	gui_renderer(const content::assets::shader& shader);

	void begin(const box<2, int>& view_size);
	void end();

	void draw_sprite(const content::assets::texture& texture, const box<2, int>& position, const box<2, float>& tex_coords, int depth = 0);
	void draw_sprite(const content::assets::texture& texture, const box<2, int>& position, const box<2, int>& tex_coords, int depth = 0);
	void draw_frame(const content::assets::texture& texture, const box<2, int>& position, const vector<2, int>& tex_coords, int depth = 0);
};

}
}
