#include "depth_render_target.h"
#include <iostream>

gorc::graphics::depth_render_target::depth_render_target(const box<2, int>& size) : render_target(size) {
    glGenFramebuffers(1, &framebuffer_name);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_name);

    glGenTextures(1, &texture_name);
    glBindTexture(GL_TEXTURE_2D, texture_name);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, get_size<0>(size), get_size<1>(size), 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture_name, 0);

    glDrawBuffer(GL_NONE);
}

void gorc::graphics::depth_render_target::release_texture() {
    glDeleteFramebuffers(1, &framebuffer_name);
    glDeleteTextures(1, &texture_name);
}

gorc::graphics::depth_render_target::~depth_render_target() {
    release_texture();
}

gorc::graphics::depth_render_target::depth_render_target(depth_render_target&& tex) : render_target(tex.m_size) {
    framebuffer_name = tex.framebuffer_name;
    texture_name = tex.texture_name;

    tex.framebuffer_name = 0;
    tex.texture_name = 0;
}

gorc::graphics::depth_render_target& gorc::graphics::depth_render_target::operator=(depth_render_target&& tex) {
    framebuffer_name = tex.framebuffer_name;
    texture_name = tex.texture_name;
    m_size = tex.m_size;

    tex.framebuffer_name = 0;
    tex.texture_name = 0;

    return *this;
}

void gorc::graphics::depth_render_target::static_set_render_target() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_name);
    glViewport(0, 0, get_size<0>(m_size), get_size<1>(m_size));
}

void gorc::graphics::depth_render_target::set_render_target() {
    static_set_render_target();
}

void gorc::graphics::depth_render_target::bind_texture() const {
    glBindTexture(GL_TEXTURE_2D, texture_name);
}
