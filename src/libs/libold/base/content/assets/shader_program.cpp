#include "shader_program.hpp"

gorc::fourcc const gorc::content::assets::vertex_program::type = "VERT"_4CC;
gorc::fourcc const gorc::content::assets::fragment_program::type = "FRAG"_4CC;

gorc::content::assets::shader_program::shader_program(GLuint program) : program(program) {
    return;
}

gorc::content::assets::shader_program::~shader_program() {
    if(program != 0) {
        glDeleteProgram(program);
        program = 0;
    }
}
