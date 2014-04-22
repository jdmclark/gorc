#include "shader_loader.h"
#include "base/content/assets/shader.h"
#include "base/diagnostics/helper.h"
#include "base/io/exception.h"
#include "base/utility/make_unique.h"
#include <vector>
#include <cstdint>

const std::vector<boost::filesystem::path> gorc::content::loaders::shader_loader::asset_root_path = { "misc/glsl" };

std::unique_ptr<gorc::content::asset> gorc::content::loaders::shader_loader::deserialize(const boost::filesystem::path& filename,
        io::read_only_file& file, content_manager&, diagnostics::report& report) {
    std::vector<char> shader_text(file.size() + 1, '\0');
    file.read(shader_text.data(), file.size());

    const char* vshader[2] = { "#define VERTEXPROGRAM\n", shader_text.data() };
    const char* fshader[2] = { "#define FRAGMENTPROGRAM\n", shader_text.data() };

    GLint result = GL_FALSE;

    auto vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderObject, 2, vshader, nullptr);
    glCompileShader(vertexShaderObject);

    auto fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObject, 2, fshader, nullptr);
    glCompileShader(fragmentShaderObject);

    bool failed = false;
    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        failed = true;

        GLint err_len;
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &err_len);
        std::vector<char> err_msg(err_len, '\0');
        glGetShaderInfoLog(vertexShaderObject, err_len, nullptr, err_msg.data());

        report.add_error("ShaderLoader", err_msg.data(), diagnostics::error_location(filename, 0, 0, 0, 0));
    }

    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        failed = true;

        GLint err_len;
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &err_len);
        std::vector<char> err_msg(err_len, '\0');
        glGetShaderInfoLog(fragmentShaderObject, err_len, nullptr, err_msg.data());

        report.add_error("ShaderLoader", err_msg.data(), diagnostics::error_location(filename, 0, 0, 0, 0));
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
            glGetProgramInfoLog(programObject, err_len, nullptr, err_msg.data());

            report.add_error("ShaderLoader", err_msg.data(), diagnostics::error_location(filename, 0, 0, 0, 0));
        }

    if(failed) {
        throw io::file_corrupt_exception();
    }

    glDeleteShader(vertexShaderObject);
    glDeleteShader(fragmentShaderObject);

    return make_unique<assets::shader>(programObject);
}
