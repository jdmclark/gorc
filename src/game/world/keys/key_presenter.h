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

class application;

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
	void update(double dt);

	std::tuple<vector<3>, vector<3>> get_node_frame(int mix_id, int node_id, flag_set<flags::mesh_node_type> node_type) const;

	int play_key(int thing_id, int key, int priority, flag_set<flags::key_flag> flags);
	int play_puppet_key(int thing_id, flags::puppet_mode_type major_mode, flags::puppet_submode_type minor_mode);

	static void register_verbs(cog::verbs::verb_table&, application&);

private:
	template <typename T, typename U> void visit_mesh_node(T& visitor, const U& thing, int mesh_id) {
		if(mesh_id < 0) {
			return;
		}

		const content::assets::model& model = *thing.model_3d;
		const content::assets::model_node& node = model.hierarchy_nodes[mesh_id];

		visitor.push_matrix();

		vector<3> anim_translate = make_zero_vector<3, float>();
		vector<3> anim_rotate = make_zero_vector<3, float>();

		if(thing.attached_key_mix >= 0) {
			std::tie(anim_translate, anim_rotate) = get_node_frame(thing.attached_key_mix, mesh_id, node.type);
		}
		else {
			anim_translate = node.offset;
			anim_rotate = node.rotation;
		}

		visitor.concatenate_matrix(make_translation_matrix(anim_translate)
				* make_rotation_matrix(get<1>(anim_rotate), make_vector(0.0f, 0.0f, 1.0f))
				* make_rotation_matrix(get<0>(anim_rotate), make_vector(1.0f, 0.0f, 0.0f))
				* make_rotation_matrix(get<2>(anim_rotate), make_vector(0.0f, 1.0f, 0.0f))
				* make_translation_matrix(node.pivot));

		if(node.mesh >= 0) {
			visitor.visit_mesh(model, node.mesh);
		}

		visitor.concatenate_matrix(make_translation_matrix(-node.pivot));

		visit_mesh_node(visitor, thing, node.child);

		visitor.pop_matrix();

		visit_mesh_node(visitor, thing, node.sibling);
	}

public:

	template <typename T, typename U> void visit_mesh_hierarchy(T& visitor, const U& thing) {
		visitor.push_matrix();
		visitor.concatenate_matrix(make_translation_matrix(thing.position)
				* make_rotation_matrix(get<1>(thing.orient), make_vector(0.0f, 0.0f, 1.0f))
				* make_rotation_matrix(get<0>(thing.orient), make_vector(1.0f, 0.0f, 0.0f))
				* make_rotation_matrix(get<2>(thing.orient), make_vector(0.0f, 1.0f, 0.0f)));

		visit_mesh_node(visitor, thing, 0);

		visitor.pop_matrix();
	}
};

}
}
}
}
