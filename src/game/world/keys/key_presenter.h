#pragma once

#include "content/flags/key_marker_type.h"
#include "content/flags/mesh_node_type.h"
#include "content/flags/puppet_mode_type.h"
#include "content/flags/puppet_submode_type.h"
#include "content/flags/key_flag.h"
#include "content/assets/model.h"
#include "framework/utility/flag_set.h"
#include "framework/math/vector.h"
#include "framework/math/matrix.h"
#include "content/assets/puppet.h"
#include "framework/math/quaternion.h"
#include "framework/utility/time.h"

namespace gorc {

namespace cog {
namespace verbs {
class verb_table;
}
}

namespace content {
class manager;
}

namespace game {

class level_state;

namespace world {

class level_model;

namespace keys {

class key_model;
class key_state;
class key_mix;

class key_presenter {
private:
    content::manager& contentmanager;
    level_model* levelModel;
    key_model* model;

    int GetThingMixId(int thing_id);
    void DispatchAllMarkers(int thing_id, const std::vector<std::tuple<double, flags::key_marker_type>>& markers,
            double begin, double end, bool wraps, double frame_ct);
    void DispatchMarker(int thing_id, flags::key_marker_type marker);

public:
    key_presenter(content::manager& contentmanager);

    void start(level_model& levelModel, key_model& model);
    void update(const time& time);

    void expunge_thing_animations(int thing_id);

    std::tuple<vector<3>, vector<3>> get_animation_frame(const content::assets::animation& anim, int node_id, double frame) const;
    std::tuple<vector<3>, vector<3>> get_node_frame(int mix_id, int node_id, flag_set<flags::mesh_node_type> node_type) const;

    int create_key_mix();

    float get_key_len(int key_id);
    int play_mix_key(int mix_id, int key, int priority, flag_set<flags::key_flag> flags);
    int play_key(int thing_id, int key, int priority, flag_set<flags::key_flag> flags);
    int play_mode(int thing_id, flags::puppet_submode_type submode);
    void stop_key(int thing_id, int key, float delay);
    void stop_all_mix_keys(int mix);

    static void register_verbs(cog::verbs::verb_table&, level_state&);

private:
    template <typename T> void visit_mesh_node(T& visitor, const content::assets::model& obj, int attached_key_mix, int mesh_id,
            content::assets::puppet const* puppet_file, float head_pitch) {
        if(mesh_id < 0) {
            return;
        }

        const content::assets::model_node& node = obj.hierarchy_nodes[mesh_id];

        visitor.push_matrix();

        vector<3> anim_translate = make_zero_vector<3, float>();
        vector<3> anim_rotate = make_zero_vector<3, float>();

        if(attached_key_mix >= 0) {
            std::tie(anim_translate, anim_rotate) = get_node_frame(attached_key_mix, mesh_id, node.type);
        }
        else {
            anim_translate = node.offset;
            anim_rotate = node.rotation;
        }

        // Add head pitch to anim_rotate:
        if(puppet_file &&
                (mesh_id == puppet_file->get_joint(flags::puppet_joint_type::head)
                || mesh_id == puppet_file->get_joint(flags::puppet_joint_type::neck)
                || mesh_id == puppet_file->get_joint(flags::puppet_joint_type::torso)
                || mesh_id == puppet_file->get_joint(flags::puppet_joint_type::primary_weapon_aiming_joint)
                || mesh_id == puppet_file->get_joint(flags::puppet_joint_type::secondary_weapon_aiming_joint)
                || mesh_id == puppet_file->get_joint(flags::puppet_joint_type::primary_weapon_fire)
                || mesh_id == puppet_file->get_joint(flags::puppet_joint_type::secondary_weapon_fire)
                )) {
            get<0>(anim_rotate) += head_pitch / 3.0f;
        }

        visitor.concatenate_matrix(make_translation_matrix(anim_translate)
                * make_rotation_matrix(get<1>(anim_rotate), make_vector(0.0f, 0.0f, 1.0f))
                * make_rotation_matrix(get<0>(anim_rotate), make_vector(1.0f, 0.0f, 0.0f))
                * make_rotation_matrix(get<2>(anim_rotate), make_vector(0.0f, 1.0f, 0.0f))
                * make_translation_matrix(node.pivot));

        if(node.mesh >= 0) {
            visitor.visit_mesh(obj, node.mesh, mesh_id);
        }

        visitor.concatenate_matrix(make_translation_matrix(-node.pivot));

        visit_mesh_node(visitor, obj, attached_key_mix, node.child, puppet_file, head_pitch);

        visitor.pop_matrix();

        visit_mesh_node(visitor, obj, attached_key_mix, node.sibling, puppet_file, head_pitch);
    }

public:

    template <typename T> void visit_mesh_hierarchy(T& visitor, const content::assets::model& obj,
            const vector<3>& base_position, const quaternion<float>& base_orientation, int attached_key_mix,
            content::assets::puppet const* puppet_file = nullptr, float head_pitch = 0.0f) {
        visitor.push_matrix();
        visitor.concatenate_matrix(make_translation_matrix(base_position)
                * convert_to_rotation_matrix(base_orientation));

        visit_mesh_node(visitor, obj, attached_key_mix, 0, puppet_file, head_pitch);

        visitor.pop_matrix();
    }
};

}
}
}
}
