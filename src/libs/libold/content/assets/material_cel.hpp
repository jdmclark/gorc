#pragma once

#include "libold/base/content/assets/texture.hpp"

namespace gorc {
namespace content {
namespace assets {

class material_cel {
public:
    texture diffuse, light;

    material_cel(GLuint diffuse, GLuint light, const box<2, int>& m_size);
    material_cel(const material_cel&) = delete;
    material_cel(material_cel&&) = default;
};

}
}
}
