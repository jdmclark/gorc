#include <GL/glew.h>
#include "thing_mesh_node_visitor.hpp"
#include "jk/content/material.hpp"
#include "libold/content/assets/model.hpp"
#include "libold/content/assets/puppet.hpp"
#include "client/world/level_view.hpp"
#include "game/world/level_presenter.hpp"
#include "game/world/keys/key_presenter.hpp"

gorc::client::world::thing_mesh_node_visitor::thing_mesh_node_visitor(client_renderer_object_factory &renderer_object_factory, const color& sector_color, level_view& view,
        int weapon_mesh_node, int saber_mesh_node_a, int saber_mesh_node_b,
        maybe<asset_ref<content::assets::model>> weapon_mesh, float saber_length, float saber_base_radius, float saber_tip_radius,
        maybe<asset_ref<material>> saber_blade, maybe<asset_ref<material>> saber_tip)
    : renderer_object_factory(renderer_object_factory), sector_color(sector_color), view(view), weapon_mesh_node(weapon_mesh_node), saber_mesh_node_a(saber_mesh_node_a),
      saber_mesh_node_b(saber_mesh_node_b), weapon_mesh(weapon_mesh), saber_length(saber_length),
      saber_base_radius(saber_base_radius), saber_tip_radius(saber_tip_radius), saber_blade(saber_blade), saber_tip(saber_tip) {
    return;
}

void gorc::client::world::thing_mesh_node_visitor::visit_mesh(asset_ref<content::assets::model> model, int mesh_id, int node_id) {
    const content::assets::model_mesh& mesh = model->geosets.front().meshes[mesh_id];
    for(const auto& face : mesh.faces) {
        if(face.material >= 0) {
            const auto& material = model->materials[face.material];

            float alpha = (face.type & flags::face_flag::Translucent) ? 0.5f : 1.0f;

            auto const &cel_size = material->cels.at(0);
            vector<2> tex_scale = make_vector(1.0f / static_cast<float>(get<0>(cel_size)),
                                              1.0f / static_cast<float>(get<1>(cel_size)));

            float light = face.extra_light;
            if(face.light == flags::light_mode::fully_lit || mesh.light == flags::light_mode::fully_lit) {
                light = 1.0f;
            }

            auto extra_lit_color = sector_color;
            for(float& ch : extra_lit_color) {
                ch = clamp(ch + light, 0.0f, 1.0f);
            }
            get<3>(extra_lit_color) = alpha;

            material_id mat_id(static_cast<int>(material.get_id()));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,
                renderer_object_factory.get_material_image(mat_id, 0, 0));
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D,
                renderer_object_factory.get_material_image(mat_id, 0, 1));

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

    if(weapon_mesh.has_value() && node_id == weapon_mesh_node) {
        thing_mesh_node_visitor weapon_mesh_node_visitor(renderer_object_factory, sector_color, view);
        view.get_presenter().key_presenter->visit_mesh_hierarchy(weapon_mesh_node_visitor, weapon_mesh.get_value(), make_zero_vector<3, float>(),
                quaternion<float>(), invalid_id);
    }

    if(saber_blade.has_value() && saber_tip.has_value() && (node_id == saber_mesh_node_a || node_id == saber_mesh_node_b)) {
        view.draw_saber(saber_tip.get_value(), saber_blade.get_value(), saber_length, saber_base_radius, saber_tip_radius);
    }
}
