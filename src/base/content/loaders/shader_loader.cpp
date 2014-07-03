#include "shader_loader.h"
#include "base/content/assets/shader.h"
#include "base/diagnostics/helper.h"
#include "base/io/exception.h"
#include "base/utility/make_unique.h"
#include "base/content/assets/shader_program.h"
#include "base/content/content_manager.h"
#include <vector>
#include <cstdint>
#include <boost/format.hpp>

using namespace gorc::content::loaders;

const std::vector<boost::filesystem::path> shader_loader::asset_root_path = { "misc/glsl" };

std::unique_ptr<gorc::content::asset> shader_loader::parse(boost::filesystem::path const &filename,
                                                           io::json_input_stream &jis,
                                                           content_manager &content,
                                                           diagnostics::report &report) {
    GLint result = GL_FALSE;

    assets::shader_program const *vs = nullptr;
    assets::shader_program const *ps = nullptr;

    io::json_deserialize_members(jis, [&](std::string const &em_name) {
        if(em_name == "vertex_program") {
            auto shader_filename = io::json_deserialize<std::string>(jis);
            vs = &content.load<assets::shader_program>(shader_filename,
                                                       GL_VERTEX_SHADER);
        }
        else if(em_name == "fragment_program") {
            auto shader_filename = io::json_deserialize<std::string>(jis);
            ps = &content.load<assets::shader_program>(shader_filename,
                                                       GL_FRAGMENT_SHADER);
        }
        else {
            report.add_error("shader_loader",
                             boost::str(boost::format("unknown shader parameter %s") % em_name),
                             diagnostics::error_location(filename, 0, 0, 0, 0));
            throw io::file_corrupt_exception();
        }
    });

    if(!vs || !ps) {
        report.add_error("shader_loader",
                         "missing shader program",
                         diagnostics::error_location(filename, 0, 0, 0, 0));
        throw io::file_corrupt_exception();
    }

    auto programObject = glCreateProgram();
    glAttachShader(programObject, vs->program);
    glAttachShader(programObject, ps->program);
    glLinkProgram(programObject);

    glGetProgramiv(programObject, GL_LINK_STATUS, &result);
    if(result == GL_FALSE) {
        GLint err_len;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &err_len);
        std::vector<char> err_msg(err_len, '\0');
        glGetProgramInfoLog(programObject, err_len, nullptr, err_msg.data());

        report.add_error("shader_loader",
                         err_msg.data(),
                         diagnostics::error_location(filename, 0, 0, 0, 0));
        throw io::file_corrupt_exception();
    }

    return make_unique<assets::shader>(programObject);
}
