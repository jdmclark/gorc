#include "log/log.hpp"
#include "shader_loader.hpp"
#include "libold/base/content/assets/shader.hpp"
#include "libold/base/content/assets/shader_program.hpp"
#include "content/content_manager.hpp"
#include <vector>
#include <cstdint>
#include <boost/format.hpp>

using namespace gorc::content::loaders;

gorc::fourcc const gorc::content::loaders::shader_loader::type = "GLSL"_4CC;

namespace {
    const std::vector<gorc::path> asset_root_path = { "misc/glsl" };
}

std::vector<gorc::path> const& shader_loader::get_prefixes() const
{
    return asset_root_path;
}

std::unique_ptr<gorc::asset> shader_loader::parse(json_input_stream &jis,
                                                  content_manager &content,
                                                  service_registry const &) const
{
    GLint result = GL_FALSE;

    maybe<asset_ref<assets::vertex_program>> vs;
    maybe<asset_ref<assets::fragment_program>> ps;

    json_deserialize_members(jis, [&](json_input_stream &jis, std::string const &em_name) {
        if(em_name == "vertex_program") {
            auto shader_filename = json_deserialize<std::string>(jis);
            vs = content.load<assets::vertex_program>(shader_filename);
        }
        else if(em_name == "fragment_program") {
            auto shader_filename = json_deserialize<std::string>(jis);
            ps = content.load<assets::fragment_program>(shader_filename);
        }
        else {
            LOG_FATAL(format("unknown shader parameter '%s'") % em_name);
        }
    });

    if(!vs.has_value() || !ps.has_value()) {
        LOG_FATAL("both 'vertex_program' and 'fragment_program' must be specified");
    }

    auto programObject = glCreateProgram();
    glAttachShader(programObject, vs.get_value()->program);
    glAttachShader(programObject, ps.get_value()->program);
    glLinkProgram(programObject);

    glGetProgramiv(programObject, GL_LINK_STATUS, &result);
    if(result == GL_FALSE) {
        GLint err_len;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &err_len);
        std::vector<char> err_msg(err_len, '\0');
        glGetProgramInfoLog(programObject, err_len, nullptr, err_msg.data());

        LOG_FATAL(format("failed to link shader: %s") % err_msg.data());
    }

    return std::make_unique<assets::shader>(programObject);
}
