#include "shader_program.hpp"

gorc::content::assets::shader_program::shader_program(GLuint program) : program(program) {
    return;
}

gorc::content::assets::shader_program::shader_program(shader_program&& sh) {
    program = sh.program;
    sh.program = 0;
}

gorc::content::assets::shader_program::~shader_program() {
    if(program != 0) {
        glDeleteProgram(program);
        program = 0;
    }
}
