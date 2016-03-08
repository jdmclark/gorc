#include "default_render_target.hpp"

gorc::graphics::default_render_target::default_render_target(const box<2, int>& size) : render_target(size) {
    return;
}

void gorc::graphics::default_render_target::static_set_render_target() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, get_size<0>(m_size), get_size<1>(m_size));
}

void gorc::graphics::default_render_target::set_render_target() {
    static_set_render_target();
}
