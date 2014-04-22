#pragma once

#include "base/utility/time.h"
#include "content/flags/autoselect_mode.h"
#include "content/flags/goal_flag.h"
#include "base/utility/flag_set.h"

namespace gorc {
namespace cog {
namespace verbs {
class verb_table;
}
}

namespace game {
class level_state;

namespace world {
class level_model;
class level_presenter;

namespace inventory {

class inventory_model;

class inventory_presenter {
private:
    level_presenter& presenter;
    inventory_model* model;
    level_model* levelModel;

public:
    inventory_presenter(level_presenter& presenter);

    void start(level_model& levelModel, inventory_model& model);
    void update(const time& time);

    void change_inv(int player, int bin, int amount);
    int get_inv_cog(int player, int bin);
    int get_inv(int player, int bin);
    int get_inv_max(int player, int bin);
    int get_inv_min(int player, int bin);
    bool is_inv_activated(int player, int bin);
    bool is_inv_available(int player, int bin);
    void set_bin_wait(int player, int bin, float delay);
    void set_inv(int player, int bin, int value);
    void set_inv_activated(int player, int bin, bool value);
    void set_inv_available(int player, int bin, bool value);

    void assign_weapon(int player, int weap_bin);
    void activate_weapon(int player, float firewait, int fire_mode);
    int autoselect_weapon(int player, flags::autoselect_mode select_mode);
    void change_fire_rate(int player, float firewait);
    float deactivate_weapon(int player, int fire_mode);
    int get_cur_weapon(int player);
    int get_cur_weapon_mode();
    void select_weapon(int player, int weap_bin);
    void set_cur_inv_weapon(int player, int bin_num);
    void set_cur_weapon(int player, int weap_num);
    void set_mount_wait(int player, float wait);

    void on_item_hotkey_pressed(int player, int bin);
    void on_item_hotkey_released(int player, int bin);

    void on_weapon_fire_pressed(int player, int mode);
    void on_weapon_fire_released(int player, int mode);

    void clear_goal_flags(int player, int goal, flag_set<flags::goal_flag> flags);
    void set_goal_flags(int player, int goal, flag_set<flags::goal_flag> flags);

    static void register_verbs(cog::verbs::verb_table& verbTable, level_state&);
};

}
}
}
}
