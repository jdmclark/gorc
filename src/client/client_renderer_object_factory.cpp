#include "client_renderer_object_factory.hpp"

gorc::client_renderer_object_factory::~client_renderer_object_factory()
{
    for(auto &material_image : material_images) {
        glDeleteTextures(1, &material_image.second);
    }
}

void gorc::client_renderer_object_factory::set_material_image(material_id id,
                                                              int cel,
                                                              int channel,
                                                              grid<color_rgba8> const &img)
{
    GLuint texture_id;
    glGenTextures(1, &texture_id);

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLfloat largest_supported_anisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest_supported_anisotropy);

    glTexImage2D(GL_TEXTURE_2D,
                 /* level */ 0,
                 GL_RGBA,
                 static_cast<GLsizei>(get<0>(img.size)),
                 static_cast<GLsizei>(get<1>(img.size)),
                 /* border */ 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 reinterpret_cast<char const *>(img.data()));

    material_images.emplace(std::make_tuple(id, cel, channel), texture_id);
}

GLuint gorc::client_renderer_object_factory::get_material_image(material_id id,
                                                                int cel,
                                                                int channel)
{
    return material_images.at(std::make_tuple(id, cel, channel));
}
