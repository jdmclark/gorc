#include <GL/glew.h>
#include "pov_mesh_node_visitor.h"
#include "content/assets/material.h"
#include "content/assets/model.h"
#include "content/assets/puppet.h"
#include "client/world/level_view.h"
#include "game/world/level_presenter.h"
#include "game/world/keys/key_presenter.h"

gorc::client::world::pov_mesh_node_visitor::pov_mesh_node_visitor(const vector<4>& sector_color, level_view& view,
        int saber_draw_node, float saber_length, float saber_base_radius, float saber_tip_radius,
        const content::assets::material* saber_blade, const content::assets::material* saber_tip)
    : sector_color(sector_color), view(view), saber_draw_node(saber_draw_node), saber_length(saber_length),
      saber_base_radius(saber_base_radius), saber_tip_radius(saber_tip_radius), saber_blade(saber_blade), saber_tip(saber_tip) {
    return;
}

void gorc::client::world::pov_mesh_node_visitor::visit_mesh(const content::assets::model& model, int mesh_id, int node_id) {
    const content::assets::model_mesh& mesh = model.geosets.front().meshes[mesh_id];
    for(const auto& face : mesh.faces) {
        if(face.material >= 0) {
            const auto& material = model.materials[face.material];

            float alpha = (face.type & flags::face_flag::Translucent) ? 0.5f : 1.0f;

            vector<2> tex_scale = make_vector(1.0f / static_cast<float>(get_size<0>(material->size)),
                    1.0f / static_cast<float>(get_size<1>(material->size)));

            float light = face.extra_light;
            if(face.light == flags::light_mode::fully_lit || mesh.light == flags::light_mode::fully_lit) {
                light = 1.0f;
            }

            auto extra_lit_color = sector_color;
            for(float& ch : extra_lit_color) {
                ch = clamp(ch + light, 0.0f, 1.0f);
            }
            get<3>(extra_lit_color) = alpha;

            glActiveTexture(GL_TEXTURE0);
            graphics::bind_texture(material->cels[0].diffuse);
            glActiveTexture(GL_TEXTURE1);
            graphics::bind_texture(material->cels[0].light);

            glBegin(GL_TRIANGLES);

            vector<3> first_geo = mesh.vertices[std::get<0>(face.vertices[0])];
            vector<2> first_tex = mesh.texture_vertices[std::get<1>(face.vertices[0])];
            vector<3> first_normal = mesh.vertex_normals[std::get<0>(face.vertices[0])];

            for(size_t i = 2; i < face.vertices.size(); ++i) {
                vector<3> second_geo = mesh.vertices[std::get<0>(face.vertices[i - 1])];
                vector<2> second_tex = mesh.texture_vertices[std::get<1>(face.vertices[i - 1])];
                vector<3> second_normal = mesh.vertex_normals[std::get<0>(face.vertices[i - 1])];

                vector<3> third_geo = mesh.vertices[std::get<0>(face.vertices[i])];
                vector<2> third_tex = mesh.texture_vertices[std::get<1>(face.vertices[i])];
                vector<3> third_normal = mesh.vertex_normals[std::get<0>(face.vertices[i])];

                apply(glNormal3f, first_normal);
                glTexCoord2f(get<0>(first_tex) * get<0>(tex_scale), get<1>(first_tex) * get<1>(tex_scale));
                apply(glColor4f, extra_lit_color);
                apply(glVertex3f, first_geo);

                apply(glNormal3f, second_normal);
                glTexCoord2f(get<0>(second_tex) * get<0>(tex_scale), get<1>(second_tex) * get<1>(tex_scale));
                apply(glColor4f, extra_lit_color);
                apply(glVertex3f, second_geo);

                apply(glNormal3f, third_normal);
                glTexCoord2f(get<0>(third_tex) * get<0>(tex_scale), get<1>(third_tex) * get<1>(tex_scale));
                apply(glColor4f, extra_lit_color);
                apply(glVertex3f, third_geo);
            }

            glEnd();
        }
    }

    if(saber_draw_node == node_id && saber_blade && saber_tip) {
        view.draw_saber(*saber_tip, *saber_blade, saber_length, saber_base_radius, saber_tip_radius);
    }
}
