#pragma once

#include "render_target.h"

namespace gorc {
namespace graphics {

class depth_render_target : public render_target {
	friend void set_render_target(depth_render_target&);
	friend void bind_texture(const depth_render_target&);

private:
	GLuint framebuffer_name;
	GLuint texture_name;

	void static_set_render_target();
	virtual void set_render_target() override;
	void bind_texture() const;
	void release_texture();

public:
	explicit depth_render_target(const box<2, int>& size);
	~depth_render_target();

	depth_render_target(depth_render_target&&);
	depth_render_target& operator=(depth_render_target&&);

	depth_render_target(const depth_render_target&) = delete;
	const depth_render_target& operator=(const depth_render_target&) = delete;
};

inline void set_render_target(depth_render_target& tex) {
	tex.static_set_render_target();
}

inline void bind_texture(const depth_render_target& tex) {
	tex.bind_texture();
}

}
}
