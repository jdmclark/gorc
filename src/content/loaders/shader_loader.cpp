#include "shader_loader.h"
#include "content/assets/shader.h"
#include "framework/diagnostics/helper.h"
#include "framework/io/exception.h"
#include <vector>
#include <cstdint>

const std::vector<boost::filesystem::path> Gorc::Content::Loaders::ShaderLoader::AssetRootPath = { "misc/glsl" };

std::unique_ptr<Gorc::Content::Asset> Gorc::Content::Loaders::ShaderLoader::Deserialize(IO::ReadOnlyFile& file, Manager& manager, Diagnostics::Report& report) {
	std::unique_ptr<Content::Assets::Shader> frag(new Content::Assets::Shader());

	std::vector<char> shader_text(file.GetSize() + 1, '\0');
	file.Read(shader_text.data(), file.GetSize());

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

		report.AddError("ShaderLoader", err_msg.data(), Diagnostics::ErrorLocation(file.Filename, 0, 0, 0, 0));
	}

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &result);
	if(result == GL_FALSE) {
		failed = true;

		GLint err_len;
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &err_len);
		std::vector<char> err_msg(err_len, '\0');
		glGetShaderInfoLog(fragmentShaderObject, err_len, NULL, err_msg.data());

		report.AddError("ShaderLoader", err_msg.data(), Diagnostics::ErrorLocation(file.Filename, 0, 0, 0, 0));
	}

	if(failed) {
		throw IO::FileCorruptException();
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

			report.AddError("ShaderLoader", err_msg.data(), Diagnostics::ErrorLocation(file.Filename, 0, 0, 0, 0));
		}

	if(failed) {
		throw IO::FileCorruptException();
	}

	glDeleteShader(vertexShaderObject);
	glDeleteShader(fragmentShaderObject);

	frag->program = programObject;

	return std::unique_ptr<Asset>(std::move(frag));
}
