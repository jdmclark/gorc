#pragma once

#include "libold/base/utility/time.hpp"
#include "libold/content/flags/autoselect_mode.hpp"
#include "libold/content/flags/goal_flag.hpp"
#include "jk/cog/script/verb_table.hpp"
#include "utility/flag_set.hpp"
#include "player_inventory.hpp"
#include "libold/content/assets/inventory.hpp"

namespace gorc {
namespace game {
class level_state;

namespace world {
class level_model;
class level_presenter;

namespace inventory {

class inventory_presenter {
private:
    level_presenter& presenter;
    asset_ref<content::assets::inventory> base_inventory;

    level_model* levelModel;

    player_inventory& get_inventory(thing_id player_id);

public:
    inventory_presenter(level_presenter& presenter,
                        asset_ref<content::assets::inventory> base_inventory);

    void start(level_model& levelModel);
    void update(const gorc::time& time);

    void change_inv(thing_id player, int bin, int amount);
    maybe<cog_id> get_inv_cog(thing_id player, int bin);
    int get_inv(thing_id player, int bin);
    int get_inv_max(thing_id player, int bin);
    int get_inv_min(thing_id player, int bin);
    bool is_inv_activated(thing_id player, int bin);
    bool is_inv_available(thing_id player, int bin);
    void set_bin_wait(thing_id player, int bin, float delay);
    void set_inv(thing_id player, int bin, int value);
    void set_inv_activated(thing_id player, int bin, bool value);
    void set_inv_available(thing_id player, int bin, bool value);

    void assign_weapon(thing_id player, int weap_bin);
    void activate_weapon(thing_id player, float firewait, int fire_mode);
    int autoselect_weapon(thing_id player, flags::autoselect_mode select_mode);
    void change_fire_rate(thing_id player, float firewait);
    float deactivate_weapon(thing_id player, int fire_mode);
    int get_cur_weapon(thing_id player);
    int get_cur_weapon_mode();
    void select_weapon(thing_id player, int weap_bin);
    void set_cur_inv_weapon(thing_id player, int bin_num);
    void set_cur_weapon(thing_id player, int weap_num);
    void set_mount_wait(thing_id player, float wait);

    void on_item_hotkey_pressed(thing_id player, int bin);
    void on_item_hotkey_released(thing_id player, int bin);

    void on_weapon_fire_pressed(thing_id player, int mode);
    void on_weapon_fire_released(thing_id player, int mode);

    void clear_goal_flags(thing_id player, int goal, flag_set<flags::goal_flag> flags);
    void set_goal_flags(thing_id player, int goal, flag_set<flags::goal_flag> flags);

    static void register_verbs(cog::verb_table& verbTable, level_state&);
};

}
}
}
}
