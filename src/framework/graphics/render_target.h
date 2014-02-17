#pragma once

#include "framework/math/box.h"
#include <GL/glew.h>

namespace gorc {
namespace graphics {

class render_target {
    friend void set_render_target(render_target&);

protected:
    box<2, int> m_size;
    virtual void set_render_target() = 0;

public:
    explicit render_target(const box<2, int>& size);
    virtual ~render_target();

    inline const box<2, int>& size() const {
        return m_size;
    }
};

inline void set_render_target(render_target& tex) {
    tex.set_render_target();
}

}
}
