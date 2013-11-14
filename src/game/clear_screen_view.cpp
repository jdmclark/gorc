#include <GL/glew.h>
#include "clear_screen_view.h"

void gorc::game::clear_screen_view::draw(const time& time, const box<2, int>& view_size, graphics::render_target&) {
	glClearDepth(1.0f);
	glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
