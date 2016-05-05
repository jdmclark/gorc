#pragma once

#include <vector>
#include <tuple>
#include "utility/flag_set.hpp"
#include "libold/content/flags/key_flag.hpp"
#include "libold/base/utility/time.hpp"
#include "jk/cog/script/verb_table.hpp"
#include "game/world/components/pov_model.hpp"

namespace gorc {

class content_manager;

namespace game {

class level_state;

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

    std::vector<std::tuple<sector_id, surface_id>> update_path_sector_scratch;

    components::pov_model& get_thing_pov_model(thing_id);

public:
    camera_presenter(level_presenter& presenter);

    void start(level_model& levelmodel, camera_model& model);
    void update(const gorc::time& time);

    void cycle_camera();
    int get_camera_state_flags();
    int get_current_camera();
    thing_id get_primary_focus(int camera_id);
    void set_camera_focus(int camera_id, thing_id focus_object_id);
    void set_camera_state_flags(int flags);
    void set_current_camera(int camera_id);
    void set_pov_shake(const vector<3>& pos_offset, const vector<3>& ang_offset, float pos_reset_speed, float ang_reset_speed);

    void jk_set_pov_model(thing_id player, model_id model);
    void jk_set_waggle(thing_id player, const vector<3>& move_vec, float speed);
    thing_id jk_play_pov_key(thing_id player, keyframe_id key, int priority, flag_set<flags::key_flag> flags);
    void jk_stop_pov_key(thing_id player, thing_id key_id, float delay);

    static void register_verbs(cog::verb_table&, level_state&);
};

}
}
}
}
