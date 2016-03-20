#include "shader.hpp"

gorc::fourcc const gorc::content::assets::shader::type = "GLSL"_4CC;

gorc::content::assets::shader::shader(GLuint program) : program(program) {
    return;
}

gorc::content::assets::shader::~shader() {
    if(program != 0) {
        glDeleteProgram(program);
        program = 0;
    }
}
