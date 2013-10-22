#include "level_shader.h"

using namespace gorc::math;

gorc::game::world::level::level_shader::level_shader(GLuint program, GLint model_mat_ul, GLint view_mat_ul, GLint proj_mat_ul)
	: program(program), model_mat_ul(model_mat_ul), view_mat_ul(view_mat_ul), proj_mat_ul(proj_mat_ul) {
	return;
}

gorc::game::world::level::level_shader::~level_shader() {
	return;
}

void gorc::game::world::level::level_shader::set_view_matrix(const matrix<4>& matrix) {
	glUniformMatrix4fv(view_mat_ul, 1, GL_FALSE, make_opengl_matrix(matrix));
}

void gorc::game::world::level::level_shader::set_projection_matrix(const matrix<4>& matrix) {
	glUniformMatrix4fv(proj_mat_ul, 1, GL_FALSE, make_opengl_matrix(matrix));
}

void gorc::game::world::level::level_shader::set_model_matrix(const matrix<4>& matrix) {
	glUniformMatrix4fv(model_mat_ul, 1, GL_FALSE, make_opengl_matrix(matrix));
}

gorc::game::world::level::surface_shader::surface_shader(const content::assets::shader& shader)
	: level_shader(shader.program, glGetUniformLocation(shader.program, "model_matrix"),
			glGetUniformLocation(shader.program, "view_matrix"),
			glGetUniformLocation(shader.program, "projection_matrix")),
	  sector_tint_ul(glGetUniformLocation(shader.program, "sector_tint")),
	  diffuse_ul(glGetUniformLocation(shader.program, "diffuse")),
	  light_ul(glGetUniformLocation(shader.program, "light")) {
	return;
}

void gorc::game::world::level::surface_shader::activate(const vector<3>& current_sector_tint) {
	glUseProgram(program);

	// Set texture locations
	glUniform1i(diffuse_ul, 0);
	glUniform1i(light_ul, 1);

	std::array<float, 4> tint_color;
	std::get<0>(tint_color) = get<0>(current_sector_tint);
	std::get<1>(tint_color) = get<1>(current_sector_tint);
	std::get<2>(tint_color) = get<2>(current_sector_tint);
	std::get<3>(tint_color) = length(current_sector_tint);

	glUniform4fv(sector_tint_ul, 1, tint_color.data());
}

gorc::game::world::level::horizon_shader::horizon_shader(const content::assets::shader& shader)
	: level_shader(shader.program, glGetUniformLocation(shader.program, "model_matrix"),
			glGetUniformLocation(shader.program, "view_matrix"),
			glGetUniformLocation(shader.program, "projection_matrix")),
	  sector_tint_ul(glGetUniformLocation(shader.program, "sector_tint")),
	  diffuse_ul(glGetUniformLocation(shader.program, "diffuse")),
	  ssz_ul(glGetUniformLocation(shader.program, "screen_size")),
	  offset_ul(glGetUniformLocation(shader.program, "horizon_sky_offset")) {
	return;
}

void gorc::game::world::level::horizon_shader::activate(const vector<3>& current_sector_tint, const vector<2>& sky_offset,
		float horizon_pixels_per_rev, float horizon_distance, const box<2, int>& screen_size, const vector<3>& camera_look) {
	glUseProgram(program);

	std::array<float, 4> tint_color;
	std::get<0>(tint_color) = get<0>(current_sector_tint);
	std::get<1>(tint_color) = get<1>(current_sector_tint);
	std::get<2>(tint_color) = get<2>(current_sector_tint);
	std::get<3>(tint_color) = length(current_sector_tint);

	std::array<float, 2> ssz { static_cast<float>(get_size<0>(screen_size)), static_cast<float>(get_size<1>(screen_size)) };

	auto horizon_start_offset = sky_offset / 256.0f;
	float horizons_per_rev = std::ceil(horizon_pixels_per_rev / 256.0f);
	float horizon_scale = 128.0f / horizon_distance;

	float yaw_revs = std::atan2(get<0>(camera_look), get<1>(camera_look)) / 6.28318f;
	get<0>(horizon_start_offset) += yaw_revs * horizons_per_rev;

	float pitch_revs = std::acos(dot(camera_look, make_vector(0.0f, 0.0f, 1.0f))) * 0.5f + 1.0f;
	get<1>(horizon_start_offset) -= pitch_revs;

	std::array<float, 3> offset = { get<0>(horizon_start_offset), get<1>(horizon_start_offset), horizon_scale };

	glUniform1i(diffuse_ul, 0);
	glUniform4fv(sector_tint_ul, 1, tint_color.data());
	glUniform2fv(ssz_ul, 1, ssz.data());
	glUniform3fv(offset_ul, 1, offset.data());
}

gorc::game::world::level::ceiling_shader::ceiling_shader(const content::assets::shader& shader)
	: level_shader(shader.program, glGetUniformLocation(shader.program, "model_matrix"),
			glGetUniformLocation(shader.program, "view_matrix"),
			glGetUniformLocation(shader.program, "projection_matrix")),
	  diffuse_ul(glGetUniformLocation(shader.program, "diffuse")),
	  sector_tint_ul(glGetUniformLocation(shader.program, "sector_tint")),
	  offset_ul(glGetUniformLocation(shader.program, "ceiling_sky_offset")) {
	return;
}

void gorc::game::world::level::ceiling_shader::activate(const vector<3>& current_sector_tint, const vector<2>& sky_offset, float ceiling_sky_z) {
	glUseProgram(program);

	std::array<float, 4> tint_color;
	std::get<0>(tint_color) = get<0>(current_sector_tint);
	std::get<1>(tint_color) = get<1>(current_sector_tint);
	std::get<2>(tint_color) = get<2>(current_sector_tint);
	std::get<3>(tint_color) = length(current_sector_tint);

	std::array<float, 3> offset = { get<0>(sky_offset), get<1>(sky_offset), ceiling_sky_z };

	glUniform1i(diffuse_ul, 0);
	glUniform4fv(sector_tint_ul, 1, tint_color.data());
	glUniform3fv(offset_ul, 1, offset.data());
}

gorc::game::world::level::light_shader::light_shader(const content::assets::shader& shader)
	: level_shader(shader.program, glGetUniformLocation(shader.program, "model_matrix"),
			glGetUniformLocation(shader.program, "view_matrix"),
			glGetUniformLocation(shader.program, "projection_matrix")),
	  diffuse_ul(glGetUniformLocation(shader.program, "diffuse")),
	  lightpos_ul(glGetUniformLocation(shader.program, "light_position")),
	  lightrad_ul(glGetUniformLocation(shader.program, "inv_light_radius_squared")),
	  lightint_ul(glGetUniformLocation(shader.program, "light_intensity")),
	  campos_ul(glGetUniformLocation(shader.program, "camera_position")) {
	return;
}

void gorc::game::world::level::light_shader::activate(const vector<3>& light_position, const vector<3>& camera_position,
		float light_radius, float light_intensity) {
	glUseProgram(program);

	std::array<float, 3> light_pos = { get<0>(light_position), get<1>(light_position), get<2>(light_position) };
	std::array<float, 3> camera_pos = { get<0>(camera_position), get<1>(camera_position), get<2>(camera_position) };

	glUniform1i(diffuse_ul, 0);
	glUniform3fv(lightpos_ul, 1, light_pos.data());
	glUniform1f(lightrad_ul, 1.0f / (light_radius * light_radius));
	glUniform1f(lightint_ul, light_intensity);
	glUniform3fv(campos_ul, 1, camera_pos.data());
}
