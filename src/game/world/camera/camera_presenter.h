#pragma once

#include <vector>
#include <tuple>

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
class level_presenter;

namespace camera {

class camera_model;
class camera_state;
class current_camera_state;

class camera_presenter {
private:
	level_presenter& presenter;
	level_model* levelmodel;
	camera_model* model;

	std::vector<std::tuple<int, int>> update_path_sector_scratch;

public:
	camera_presenter(level_presenter& presenter);

	void start(level_model& levelmodel, camera_model& model);
	void update(double dt);

	void cycle_camera();
	int get_camera_state_flags();
	int get_current_camera();
	int get_primary_focus(int camera_id);
	void set_camera_focus(int camera_id, int focus_object_id);
	void set_camera_state_flags(int flags);
	void set_current_camera(int camera_id);
	void set_pov_shake(const vector<3>& pos_offset, const vector<3>& ang_offset, float pos_reset_speed, float ang_reset_speed);

	static void register_verbs(cog::verbs::verb_table&, application&);
};

}
}
}
}
