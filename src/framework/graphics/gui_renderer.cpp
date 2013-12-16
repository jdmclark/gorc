#include "gui_renderer.h"
#include "framework/math/matrix.h"

gorc::graphics::gui_renderer::gui_shader::gui_shader(const content::assets::shader& shader)
	: program(shader.program),
	  model_mat_ul(glGetUniformLocation(shader.program, "model_matrix")),
	  view_mat_ul(glGetUniformLocation(shader.program, "view_matrix")),
	  proj_mat_ul(glGetUniformLocation(shader.program, "projection_matrix")),
	  diffuse_ul(glGetUniformLocation(shader.program, "diffuse")) {
	return;
}

void gorc::graphics::gui_renderer::gui_shader::set_view_matrix(const matrix<4, float>& view_matrix) const {
	glUniformMatrix4fv(view_mat_ul, 1, GL_FALSE, make_opengl_matrix(view_matrix));
}

void gorc::graphics::gui_renderer::gui_shader::set_projection_matrix(const matrix<4, float>& proj_matrix) const {
	glUniformMatrix4fv(proj_mat_ul, 1, GL_FALSE, make_opengl_matrix(proj_matrix));
}

void gorc::graphics::gui_renderer::gui_shader::set_model_matrix(const matrix<4, float>& model_matrix) const {
	glUniformMatrix4fv(model_mat_ul, 1, GL_FALSE, make_opengl_matrix(model_matrix));
}

void gorc::graphics::gui_renderer::gui_shader::activate() const {
	glUseProgram(program);

	glUniform1i(diffuse_ul, 0);
}


gorc::graphics::gui_renderer::gui_renderer(const content::assets::shader& shader) : shader(shader) {
	return;
}

void gorc::graphics::gui_renderer::begin(const box<2, int>& view_size) {
	shader.activate();
	shader.set_projection_matrix(
			make_translation_matrix(make_vector(-1.0f, 1.0f, 0.0f)) *
			make_scale_matrix(make_vector(2.0f / static_cast<float>(get_size<0>(view_size)),
					-2.0f / static_cast<float>(get_size<1>(view_size)),
					1.0f, 1.0f)));
	shader.set_view_matrix(make_identity_matrix<4, float>());
	shader.set_model_matrix(make_identity_matrix<4, float>());

	sprites.clear();
}

void gorc::graphics::gui_renderer::end() {
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);

	glBegin(GL_TRIANGLES);

	content::assets::texture const* prev_texture = nullptr;
	for(const auto& sprite : sprites) {
		content::assets::texture const* next_tx = std::get<1>(sprite.first);
		if(prev_texture != next_tx) {
			glEnd();

			prev_texture = next_tx;
			bind_texture(*next_tx);

			glBegin(GL_TRIANGLES);
		}

		int s_left, s_right, s_top, s_bottom;
		float t_left, t_right, t_top, t_bottom;

		std::tie(s_left, s_right) = get_range<0>(std::get<0>(sprite.second));
		std::tie(s_top, s_bottom) = get_range<1>(std::get<0>(sprite.second));
		std::tie(t_left, t_right) = get_range<0>(std::get<1>(sprite.second));
		std::tie(t_top, t_bottom) = get_range<1>(std::get<1>(sprite.second));

		glTexCoord2f(t_left, t_bottom);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(s_left, s_bottom, 0.0f);

		glTexCoord2f(t_left, t_top);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(s_left, s_top, 0.0f);

		glTexCoord2f(t_right, t_top);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(s_right, s_top, 0.0f);

		glTexCoord2f(t_left, t_bottom);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(s_left, s_bottom, 0.0f);

		glTexCoord2f(t_right, t_top);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(s_right, s_top, 0.0f);

		glTexCoord2f(t_right, t_bottom);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(s_right, s_bottom, 0.0f);
	}

	glEnd();
}

void gorc::graphics::gui_renderer::draw_sprite(const content::assets::texture& texture,
		const box<2, int>& position, const box<2, float>& tex_coords, int depth) {
	sprites.insert(std::make_pair(std::make_tuple(depth, &texture), std::make_tuple(position, tex_coords)));
}

void gorc::graphics::gui_renderer::draw_sprite(const content::assets::texture& texture,
		const box<2, int>& position, const box<2, int>& tex_coords, int depth) {
	int left, right, top, bottom;
	std::tie(left, right) = get_range<0>(tex_coords);
	std::tie(top, bottom) = get_range<1>(tex_coords);
	float x_fac = 1.0f / static_cast<float>(get_size<0>(texture.size()));
	float y_fac = 1.0f / static_cast<float>(get_size<1>(texture.size()));

	draw_sprite(texture, position, make_box(make_vector(static_cast<float>(left) * x_fac, static_cast<float>(top) * y_fac),
			make_vector(static_cast<float>(right) * x_fac, static_cast<float>(bottom) * y_fac)), depth);
}

void gorc::graphics::gui_renderer::draw_frame(const content::assets::texture& texture,
		const box<2, int>& position, const vector<2, int>& part_offset, int depth) {
	matrix<3, float> txt = make_scale_matrix(make_vector(1.0f / static_cast<float>(get_size<0>(texture.size())),
			1.0f / static_cast<float>(get_size<1>(texture.size())), 1.0f))
					* make_translation_matrix(make_vector<float>(get<0>(part_offset), get<1>(part_offset)));

	constexpr int corner_size = 2;
	auto top_left = make_box(txt.transform(make_vector(0.0f, 0.0f)), txt.transform(make_vector(2.0f, 2.0f)));
	auto top_right = make_box(txt.transform(make_vector(6.0f, 0.0f)), txt.transform(make_vector(8.0f, 2.0f)));
	auto bottom_left = make_box(txt.transform(make_vector(0.0f, 6.0f)), txt.transform(make_vector(2.0f, 8.0f)));
	auto bottom_right = make_box(txt.transform(make_vector(6.0f, 6.0f)), txt.transform(make_vector(8.0f, 8.0f)));
	auto left = make_box(txt.transform(make_vector(0.0f, 2.0f)), txt.transform(make_vector(2.0f, 6.0f)));
	auto right = make_box(txt.transform(make_vector(6.0f, 2.0f)), txt.transform(make_vector(8.0f, 6.0f)));
	auto top = make_box(txt.transform(make_vector(2.0f, 0.0f)), txt.transform(make_vector(6.0f, 2.0f)));
	auto bottom = make_box(txt.transform(make_vector(2.0f, 6.0f)), txt.transform(make_vector(6.0f, 8.0f)));
	auto fill = make_box(txt.transform(make_vector(2.0f, 2.0f)), txt.transform(make_vector(6.0f, 6.0f)));

	int box_left, box_right, box_top, box_bottom;
	std::tie(box_left, box_right) = get_range<0>(position);
	std::tie(box_top, box_bottom) = get_range<1>(position);

	draw_sprite(texture, make_box(make_vector(box_left, box_top), make_vector(box_left + corner_size, box_top + corner_size)), top_left, depth);
	draw_sprite(texture, make_box(make_vector(box_left, box_bottom - corner_size), make_vector(box_left + corner_size, box_bottom)), bottom_left, depth);
	draw_sprite(texture, make_box(make_vector(box_right - corner_size, box_top), make_vector(box_right, box_top + corner_size)), top_right, depth);
	draw_sprite(texture, make_box(make_vector(box_right - corner_size, box_bottom - corner_size), make_vector(box_right, box_bottom)), bottom_right, depth);
	draw_sprite(texture, make_box(make_vector(box_left, box_top + corner_size), make_vector(box_left + corner_size, box_bottom - corner_size)), left, depth);
	draw_sprite(texture, make_box(make_vector(box_left + corner_size, box_top), make_vector(box_right - corner_size, box_top + corner_size)), top, depth);
	draw_sprite(texture, make_box(make_vector(box_right - corner_size, box_top + corner_size), make_vector(box_right, box_bottom - corner_size)), right, depth);
	draw_sprite(texture, make_box(make_vector(box_left + corner_size, box_bottom - corner_size), make_vector(box_right - corner_size, box_bottom)), bottom, depth);

	draw_sprite(texture, make_box(make_vector(box_left + corner_size, box_top + corner_size), make_vector(box_right - corner_size, box_bottom - corner_size)), fill, depth);
}
