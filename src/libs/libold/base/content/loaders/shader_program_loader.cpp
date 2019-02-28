#include "shader_program_loader.hpp"
#include "io/memory_file.hpp"
#include "libold/base/content/assets/shader_program.hpp"
#include "log/log.hpp"
#include <GL/glew.h>
#include <cstdint>
#include <vector>

using gorc::content::loaders::fragment_program_loader;
using gorc::content::loaders::shader_program_loader;
using gorc::content::loaders::vertex_program_loader;

gorc::fourcc const vertex_program_loader::type = "VERT"_4CC;
gorc::fourcc const fragment_program_loader::type = "FRAG"_4CC;

namespace {
    const std::vector<gorc::path> asset_root_path = {"misc/glsl"};
}

std::vector<gorc::path> const &shader_program_loader::get_prefixes() const
{
    return asset_root_path;
}

namespace {

    template <typename AssetT>
    std::unique_ptr<gorc::asset> deserialize_shader_program(GLuint program_type,
                                                            gorc::input_stream &file,
                                                            gorc::content_manager &,
                                                            gorc::service_registry const &)
    {
        gorc::memory_file shader_program_text;
        file.copy_to(shader_program_text);
        char null_delimiter = '\0';
        shader_program_text.write(&null_delimiter, sizeof(char));

        GLint result = GL_FALSE;

        char const *program_text = shader_program_text.data();

        auto shaderObject = glCreateShader(program_type);
        glShaderSource(shaderObject,
                       /*size*/ 1,
                       &program_text,
                       nullptr);
        glCompileShader(shaderObject);

        glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);
        if(result == GL_FALSE) {
            GLint err_len;
            glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &err_len);
            std::vector<char> err_msg(err_len, '\0');
            glGetShaderInfoLog(shaderObject, err_len, nullptr, err_msg.data());

            LOG_FATAL(gorc::format("failed to compile shader: %s") % err_msg.data());
        }

        return std::make_unique<AssetT>(shaderObject);
    }
}

std::unique_ptr<gorc::asset>
    gorc::content::loaders::vertex_program_loader::deserialize(input_stream &file,
                                                               content_manager &content,
                                                               asset_id,
                                                               service_registry const &services,
                                                               std::string const & /*name*/) const
{
    return deserialize_shader_program<assets::vertex_program>(
        GL_VERTEX_SHADER, file, content, services);
}

std::unique_ptr<gorc::asset>
    gorc::content::loaders::fragment_program_loader::deserialize(input_stream &file,
                                                                 content_manager &content,
                                                                 asset_id,
                                                                 service_registry const &services,
                                                                 std::string const & /*name*/) const
{
    return deserialize_shader_program<assets::fragment_program>(
        GL_FRAGMENT_SHADER, file, content, services);
}
