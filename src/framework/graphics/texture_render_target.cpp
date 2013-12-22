#include "texture_render_target.h"
#include <iostream>

gorc::graphics::texture_render_target::texture_render_target(const box<2, int>& size) : render_target(size) {
	glGenFramebuffers(1, &framebuffer_name);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_name);

	glGenTextures(1, &texture_name);
	glBindTexture(GL_TEXTURE_2D, texture_name);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, get_size<0>(size), get_size<1>(size), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenRenderbuffers(1, &depthbuffer_name);
	glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer_name);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, get_size<0>(size), get_size<1>(size));
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer_name);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_name, 0);

	GLenum draw_buffers[2] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, draw_buffers);
}

void gorc::graphics::texture_render_target::release_texture() {
	glDeleteFramebuffers(1, &framebuffer_name);
	glDeleteTextures(1, &texture_name);
	glDeleteRenderbuffers(1, &depthbuffer_name);
}

gorc::graphics::texture_render_target::~texture_render_target() {
	release_texture();
}

gorc::graphics::texture_render_target::texture_render_target(texture_render_target&& tex) : render_target(tex.m_size) {
	framebuffer_name = tex.framebuffer_name;
	texture_name = tex.texture_name;
	depthbuffer_name = tex.depthbuffer_name;

	tex.framebuffer_name = 0;
	tex.texture_name = 0;
	tex.depthbuffer_name = 0;
}

gorc::graphics::texture_render_target& gorc::graphics::texture_render_target::operator=(texture_render_target&& tex) {
	framebuffer_name = tex.framebuffer_name;
	texture_name = tex.texture_name;
	depthbuffer_name = tex.depthbuffer_name;
	m_size = tex.m_size;

	tex.framebuffer_name = 0;
	tex.texture_name = 0;
	tex.depthbuffer_name = 0;

	return *this;
}

void gorc::graphics::texture_render_target::static_set_render_target() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_name);
	glViewport(0, 0, get_size<0>(m_size), get_size<1>(m_size));
}

void gorc::graphics::texture_render_target::set_render_target() {
	static_set_render_target();
}

void gorc::graphics::texture_render_target::bind_texture() const {
	glBindTexture(GL_TEXTURE_2D, texture_name);
}
