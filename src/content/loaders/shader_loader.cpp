#include "shader_loader.h"
#include "content/assets/shader.h"
#include "framework/diagnostics/helper.h"
#include "framework/io/exception.h"
#include <vector>
#include <cstdint>

const std::vector<boost::filesystem::path> gorc::content::loaders::shader_loader::asset_root_path = { "misc/glsl" };

std::unique_ptr<gorc::content::asset> gorc::content::loaders::shader_loader::deserialize(io::read_only_file& file, manager& manager, diagnostics::report& report) {
	std::unique_ptr<content::assets::shader> frag(new content::assets::shader());

	std::vector<char> shader_text(file.get_size() + 1, '\0');
	file.read(shader_text.data(), file.get_size());

	const char* vshader[2] = { "#define VERTEXPROGRAM\n", shader_text.data() };
	const char* fshader[2] = { "#define FRAGMENTPROGRAM\n", shader_text.data() };

	GLint result = GL_FALSE;

	auto vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 2, vshader, NULL);
	glCompileShader(vertexShaderObject);

	auto fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 2, fshader, NULL);
	glCompileShader(fragmentShaderObject);

	bool failed = false;
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &result);
	if(result == GL_FALSE) {
		failed = true;

		GLint err_len;
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &err_len);
		std::vector<char> err_msg(err_len, '\0');
		glGetShaderInfoLog(vertexShaderObject, err_len, NULL, err_msg.data());

		report.add_error("ShaderLoader", err_msg.data(), diagnostics::error_location(file.Filename, 0, 0, 0, 0));
	}

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &result);
	if(result == GL_FALSE) {
		failed = true;

		GLint err_len;
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &err_len);
		std::vector<char> err_msg(err_len, '\0');
		glGetShaderInfoLog(fragmentShaderObject, err_len, NULL, err_msg.data());

		report.add_error("ShaderLoader", err_msg.data(), diagnostics::error_location(file.Filename, 0, 0, 0, 0));
	}

	if(failed) {
		throw io::file_corrupt_exception();
	}

	auto programObject = glCreateProgram();
	glAttachShader(programObject, vertexShaderObject);
	glAttachShader(programObject, fragmentShaderObject);
	glLinkProgram(programObject);

	glGetProgramiv(programObject, GL_LINK_STATUS, &result);
	if(result == GL_FALSE) {
			failed = true;

			GLint err_len;
			glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &err_len);
			std::vector<char> err_msg(err_len, '\0');
			glGetProgramInfoLog(programObject, err_len, NULL, err_msg.data());

			report.add_error("ShaderLoader", err_msg.data(), diagnostics::error_location(file.Filename, 0, 0, 0, 0));
		}

	if(failed) {
		throw io::file_corrupt_exception();
	}

	glDeleteShader(vertexShaderObject);
	glDeleteShader(fragmentShaderObject);

	frag->program = programObject;

	return std::unique_ptr<asset>(std::move(frag));
}
