#pragma once

#include <GL/glew.h>
#include "content/asset.hpp"
#include "libold/base/content/loaders/shader_loader.hpp"

namespace gorc {
namespace content {
namespace assets {

class shader : public asset {
public:
    using loader = loaders::shader_loader;

    GLuint program = 0;

    shader(GLuint program);
    shader(const shader&) = delete;
    shader(shader&&);
    ~shader();

    const shader& operator=(const shader&) = delete;
};

}
}
}
