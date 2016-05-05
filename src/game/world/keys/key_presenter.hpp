#pragma once

#include "libold/content/flags/key_marker_type.hpp"
#include "libold/content/flags/mesh_node_type.hpp"
#include "libold/content/flags/puppet_mode_type.hpp"
#include "libold/content/flags/puppet_submode_type.hpp"
#include "libold/content/flags/key_flag.hpp"
#include "libold/content/assets/model.hpp"
#include "utility/flag_set.hpp"
#include "math/vector.hpp"
#include "math/matrix.hpp"
#include "libold/content/assets/puppet.hpp"
#include "math/quaternion.hpp"
#include "libold/base/utility/time.hpp"
#include "utility/event_bus.hpp"
#include "jk/cog/script/verb_table.hpp"
#include "content/id.hpp"
#include "components/key_mix.hpp"
#include "components/pov_key_mix.hpp"

namespace gorc {

class content_manager;

namespace game {

class level_state;

namespace world {

class level_model;

namespace keys {

class key_state;
class key_mix;

class key_presenter {
private:
    content_manager& contentmanager;
    level_model* levelModel;
    event_bus* bus;

    key_mix& get_thing_mix(thing_id) const;
    pov_key_mix& get_thing_pov_mix(thing_id) const;

    maybe<key_mix const *> maybe_get_mix(thing_id, bool is_pov) const;

    void DispatchAllMarkers(thing_id, const std::vector<std::tuple<double, flags::key_marker_type>>& markers,
            double begin, double end, bool wraps, double frame_ct);
    void DispatchMarker(thing_id, flags::key_marker_type marker);

    void update_key_mix(key_mix &mix, double dt);
    void update_key(thing_id tid, thing_id key_id, key_state &key, key_mix &mix, double dt);

public:
    key_presenter(content_manager& contentmanager);

    void start(level_model& levelModel, event_bus& bus);
    void update(const gorc::time& time);

    void expunge_thing_animations(thing_id);

    std::tuple<vector<3>, vector<3>> get_animation_frame(asset_ref<content::assets::animation> anim, int node_id, double frame) const;
    std::tuple<vector<3>, vector<3>> get_node_frame(key_mix const &mix, int node_id, flag_set<flags::mesh_node_type> node_type) const;

    float get_key_len(keyframe_id key_id);
    thing_id play_mix_key(thing_id mix_id, bool is_pov, keyframe_id key, int priority, flag_set<flags::key_flag> flags);
    thing_id play_key(thing_id, keyframe_id key, int priority, flag_set<flags::key_flag> flags);
    thing_id play_mode(thing_id, flags::puppet_submode_type submode);
    void stop_key(thing_id, thing_id key, float delay);
    void stop_all_mix_keys(thing_id mix, bool is_pov);

    static void register_verbs(cog::verb_table&, level_state&);

private:
    template <typename T>
    void visit_mesh_node(T& visitor,
                         asset_ref<content::assets::model> obj,
                         maybe<key_mix const *> attached_key_mix,
                         int mesh_id,
                         maybe<asset_ref<content::assets::puppet>> puppet_file,
                         float head_pitch)
    {
        if(mesh_id < 0) {
            return;
        }

        const content::assets::model_node& node = obj->hierarchy_nodes[mesh_id];

        visitor.push_matrix();

        vector<3> anim_translate = make_zero_vector<3, float>();
        vector<3> anim_rotate = make_zero_vector<3, float>();

        if(attached_key_mix.has_value()) {
            std::tie(anim_translate, anim_rotate) =
                get_node_frame(*attached_key_mix.get_value(), mesh_id, node.type);
        }
        else {
            anim_translate = node.offset;
            anim_rotate = node.rotation;
        }

        // Add head pitch to anim_rotate:
        maybe_if(puppet_file, [&](auto puppet_file) {
            if(mesh_id == puppet_file->get_joint(flags::puppet_joint_type::head) ||
               mesh_id == puppet_file->get_joint(flags::puppet_joint_type::neck) ||
               mesh_id == puppet_file->get_joint(flags::puppet_joint_type::torso) ||
               mesh_id == puppet_file->get_joint(flags::puppet_joint_type::primary_weapon_aiming_joint) ||
               mesh_id == puppet_file->get_joint(flags::puppet_joint_type::secondary_weapon_aiming_joint) ||
               mesh_id == puppet_file->get_joint(flags::puppet_joint_type::primary_weapon_fire) ||
               mesh_id == puppet_file->get_joint(flags::puppet_joint_type::secondary_weapon_fire)) {
                get<0>(anim_rotate) += head_pitch / 3.0f;
            }
        });

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

    template <typename T>
    void visit_mesh_hierarchy(T& visitor,
                              asset_ref<content::assets::model> obj,
                              const vector<3>& base_position,
                              const quaternion<float>& base_orientation,
                              thing_id attached_key_mix,
                              bool is_pov_mix,
                              maybe<asset_ref<content::assets::puppet>> puppet_file = nothing,
                              float head_pitch = 0.0f)
    {
        visitor.push_matrix();
        visitor.concatenate_matrix(make_translation_matrix(base_position)
                * convert_to_rotation_matrix(base_orientation));

        maybe<key_mix const *> mix = nothing;
        if(attached_key_mix.is_valid()) {
            mix = maybe_get_mix(attached_key_mix, is_pov_mix);
        }

        visit_mesh_node(visitor, obj, mix, 0, puppet_file, head_pitch);

        visitor.pop_matrix();
    }
};

}
}
}
}
