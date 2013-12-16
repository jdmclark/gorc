#pragma once

#include "framework/content/asset.h"
#include "framework/math/box.h"
#include <SFML/Graphics.hpp>
#include <GL/glew.h>

namespace gorc {

namespace content {
namespace assets {
class texture;
}
}

namespace graphics {
void bind_texture(const content::assets::texture& tex);
}

namespace content {
namespace assets {

class texture : public asset {
	friend void gorc::graphics::bind_texture(const texture& tex);

private:
	void bind_texture() const;

	GLuint data = GL_INVALID_VALUE;
	box<2, int> m_size = make_box(make_vector(0, 0), make_vector(0, 0));

public:
	texture(GLuint data, const box<2, int>& m_size);
	texture(const texture&) = delete;
	texture(texture&&);
	~texture();

	const texture& operator=(const texture&) = delete;

	inline const box<2, int>& size() const {
		return m_size;
	}

	inline GLuint get_data() const {
		return data;
	}
};

}
}
}
