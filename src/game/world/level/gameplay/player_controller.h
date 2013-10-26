#pragma once

#include "character_controller.h"
#include "game/world/level/physics/shape.h"
#include <vector>
#include <set>
#include <stack>
#include "framework/math/matrix.h"
#include "content/assets/model.h"

namespace gorc {
namespace game {
namespace world {
namespace level {

namespace physics {
class contact_range_consumer;
}

namespace gameplay {

class player_controller : public character_controller {
	std::vector<vector<3>> resting_manifolds;
	std::set<int> closed_set;

	class node_visitor {
	private:
		std::vector<vector<3>>& resting_manifolds;
		std::stack<matrix<4>> matrices;
		matrix<4> current_matrix = make_identity_matrix<4>();

	public:
		node_visitor(std::vector<vector<3>>& resting_manifolds);

		inline void push_matrix() {
			matrices.push(current_matrix);
		}

		inline void pop_matrix() {
			current_matrix = matrices.top();
			matrices.pop();
		}

		inline void concatenate_matrix(const matrix<4>& mat) {
			current_matrix = current_matrix * mat;
		}

		void visit_mesh(const content::assets::model& model, int mesh_id);

		physics::sphere sphere;
	} anim_node_visitor;

	void find_sector_resting_manifolds(const physics::sphere& sphere, int sector_id, std::set<int>& closed, const vector<3>& vel_dir);
	void find_thing_resting_manifolds(const physics::sphere& sphere, std::set<int>& closed, const vector<3>& vel_dir, int current_thing_id);

	void step_physics(int thing_id, double dt);

public:
	player_controller(level_presenter& presenter);

	virtual void update(int thing_id, double dt) override;
};

}
}
}
}
}
