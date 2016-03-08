#include <GL/glew.h>
#include "log/log.hpp"
#include "shader_program_loader.hpp"
#include "libold/base/content/assets/shader_program.hpp"
#include "io/memory_file.hpp"
#include <vector>
#include <cstdint>

using gorc::content::loaders::shader_program_loader;
using gorc::content::loaders::vertex_program_loader;
using gorc::content::loaders::fragment_program_loader;

const std::vector<boost::filesystem::path> shader_program_loader::asset_root_path = { "misc/glsl" };

std::unique_ptr<gorc::asset> shader_program_loader::deserialize(
        input_stream &file,
        content_manager &,
        service_registry const &) const
{
    memory_file shader_program_text;
    file.copy_to(shader_program_text);
    char null_delimiter = '\0';
    shader_program_text.write(&null_delimiter, sizeof(char));

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

        LOG_FATAL(format("failed to compile shader: %s") % err_msg.data());
    }

    return std::make_unique<assets::shader_program>(shaderObject);
}

shader_program_loader::shader_program_loader(GLuint program_type)
    : program_type(program_type) {
    return;
}

vertex_program_loader::vertex_program_loader()
    : shader_program_loader(GL_VERTEX_SHADER)
{
    return;
}

fragment_program_loader::fragment_program_loader()
    : shader_program_loader(GL_FRAGMENT_SHADER)
{
    return;
}
