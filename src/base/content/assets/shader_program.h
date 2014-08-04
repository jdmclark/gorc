#pragma once

#include <GL/glew.h>
#include "base/content/asset.h"
#include "base/content/loaders/shader_program_loader.h"

namespace gorc {
namespace content {
namespace assets {

class shader_program : public asset {
public:
    using loader = loaders::shader_program_loader;

    GLuint program = 0;

    shader_program(GLuint program);
    shader_program(const shader_program&) = delete;
    shader_program(shader_program&&);
    ~shader_program();

    const shader_program& operator=(const shader_program&) = delete;
};

}
}
}
