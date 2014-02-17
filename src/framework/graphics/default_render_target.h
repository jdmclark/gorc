#pragma once

#include "render_target.h"

namespace gorc {
namespace graphics {

class default_render_target : public render_target {
    friend void set_render_target(default_render_target&);

private:
    void static_set_render_target();
    virtual void set_render_target() override;

public:
    explicit default_render_target(const box<2, int>& size);
};

inline void set_render_target(default_render_target& tex) {
    tex.static_set_render_target();
}

}
}
