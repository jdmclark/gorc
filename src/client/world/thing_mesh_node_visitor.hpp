#pragma once

#include "math/vector.hpp"
#include "client/world/level_view.hpp"

namespace gorc {

namespace content {
namespace assets {
class model;
class puppet;
class material;
}
}

namespace client {
namespace world {

class thing_mesh_node_visitor {
private:
    vector<4> sector_color;
    level_view& view;
    int weapon_mesh_node = -1;
    int saber_mesh_node_a = -1;
    int saber_mesh_node_b = -1;
    maybe<asset_ref<content::assets::model>> weapon_mesh;
    float saber_length = 0.0f;
    float saber_base_radius = 0.0f;
    float saber_tip_radius = 0.0f;
    maybe<asset_ref<content::assets::material>> saber_blade;
    maybe<asset_ref<content::assets::material>> saber_tip;

public:
    thing_mesh_node_visitor(const vector<4>& sector_color, level_view& view,
            int weapon_mesh_node = -1, int saber_mesh_node_a = -1, int saber_mesh_node_b = -1,
            maybe<asset_ref<content::assets::model>> weapon_mesh = nothing, float saber_length = 0.0f,
            float saber_base_radius = 0.0f, float saber_tip_radius = 0.0f,
            maybe<asset_ref<content::assets::material>> saber_blade = nothing, maybe<asset_ref<content::assets::material>> saber_tip = nothing);

    inline void concatenate_matrix(const matrix<4>& mat) {
        view.concatenate_matrix(mat);
        view.update_shader_model_matrix();
    }

    inline void push_matrix() {
        view.push_matrix();
    }

    inline void pop_matrix() {
        view.pop_matrix();
        view.update_shader_model_matrix();
    }

    void visit_mesh(asset_ref<content::assets::model> model, int mesh_id, int node_id);
};

}
}
}
