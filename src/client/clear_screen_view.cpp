#include <GL/glew.h>
#include "clear_screen_view.hpp"

void gorc::client::clear_screen_view::draw(const gorc::time&, const box<2, int>&, graphics::render_target&) {
    glClearDepth(1.0f);
    glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
