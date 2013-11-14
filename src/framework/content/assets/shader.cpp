#include "shader.h"

gorc::content::assets::shader::shader(GLuint program) : program(program) {
	return;
}

gorc::content::assets::shader::shader(shader&& sh) {
	program = sh.program;
	sh.program = 0;
}

gorc::content::assets::shader::~shader() {
	if(program != 0) {
		glDeleteProgram(program);
		program = 0;
	}
}
