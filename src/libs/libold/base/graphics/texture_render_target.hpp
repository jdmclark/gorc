#pragma once

#include "render_target.hpp"

namespace gorc {
namespace graphics {

class texture_render_target : public render_target {
    friend void set_render_target(texture_render_target&);
    friend void bind_texture(const texture_render_target&);

private:
    GLuint framebuffer_name;
    GLuint depthbuffer_name;
    GLuint texture_name;

    void static_set_render_target();
    virtual void set_render_target() override;
    void bind_texture() const;
    void release_texture();

public:
    explicit texture_render_target(const box<2, int>& size);
    ~texture_render_target();

    texture_render_target(texture_render_target&&);
    texture_render_target& operator=(texture_render_target&&);

    texture_render_target(const texture_render_target&) = delete;
    const texture_render_target& operator=(const texture_render_target&) = delete;
};

inline void set_render_target(texture_render_target& tex) {
    tex.static_set_render_target();
}

inline void bind_texture(const texture_render_target& tex) {
    tex.bind_texture();
}

}
}
