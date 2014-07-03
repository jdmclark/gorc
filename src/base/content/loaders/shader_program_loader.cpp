#include <GL/glew.h>
#include "shader_program_loader.h"
#include "base/content/assets/shader_program.h"
#include "base/diagnostics/helper.h"
#include "base/io/exception.h"
#include "base/utility/make_unique.h"
#include <vector>
#include <cstdint>

using gorc::content::loaders::shader_program_loader;

const std::vector<boost::filesystem::path> shader_program_loader::asset_root_path = { "misc/glsl" };

std::unique_ptr<gorc::content::asset> shader_program_loader::deserialize(
        boost::filesystem::path const &filename,
        io::read_only_file& file, content_manager&, diagnostics::report& report) {
    std::vector<char> shader_program_text(file.size() + 1, '\0');
    file.read(shader_program_text.data(), file.size());

    GLint result = GL_FALSE;

    char const *program_text = shader_program_text.data();

    auto shaderObject = glCreateShader(program_type);
    glShaderSource(shaderObject,
                   /*size*/1,
                   &program_text,
                   nullptr);
    glCompileShader(shaderObject);

    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        GLint err_len;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &err_len);
        std::vector<char> err_msg(err_len, '\0');
        glGetShaderInfoLog(shaderObject, err_len, nullptr, err_msg.data());

        report.add_error("shader_program_loader",
                         err_msg.data(),
                         diagnostics::error_location(filename, 0, 0, 0, 0));

        throw io::file_corrupt_exception();
    }

    return make_unique<assets::shader_program>(shaderObject);
}

shader_program_loader::shader_program_loader(GLuint program_type)
    : program_type(program_type) {
    return;
}
