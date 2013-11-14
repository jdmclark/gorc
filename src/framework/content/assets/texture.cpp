#include "texture.h"

void gorc::content::assets::texture::bind_texture() const {
	glBindTexture(GL_TEXTURE_2D, data);
}

void gorc::graphics::bind_texture(const content::assets::texture& tex) {
	glBindTexture(GL_TEXTURE_2D, tex.data);
}

gorc::content::assets::texture::texture(GLuint data, const box<2, int>& m_size)
	: data(data), m_size(m_size) {
	return;
}

gorc::content::assets::texture::texture(texture&& tex) {
	data = tex.data;
	m_size = tex.m_size;

	tex.data = GL_INVALID_VALUE;
}

gorc::content::assets::texture::~texture() {
	if(data != GL_INVALID_VALUE) {
		glDeleteTextures(1, &data);
		data = GL_INVALID_VALUE;
	}
}
