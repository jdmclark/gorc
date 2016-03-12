#pragma once

#include <GL/glew.h>
#include "content/asset.hpp"
#include "libold/base/content/loaders/shader_program_loader.hpp"

namespace gorc {
namespace content {
namespace assets {

class shader_program : public asset {
public:
    GLuint program = 0;

    shader_program(GLuint program);
    shader_program(const shader_program&) = delete;
    shader_program(shader_program&&);
    ~shader_program();

    const shader_program& operator=(const shader_program&) = delete;
};

class vertex_program : public shader_program {
public:
    static fourcc const type;
};

class fragment_program : public shader_program {
public:
    static fourcc const type;
};

}
}
}
