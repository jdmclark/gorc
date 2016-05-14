#pragma once

#include <unordered_map>
#include "jk/content/inventory.hpp"
#include "player_bin.hpp"

namespace gorc {
namespace game {
namespace world {
namespace inventory {

class player_inventory {
private:
    asset_ref<gorc::inventory> BaseInventory;
    std::unordered_map<int, player_bin> bins;

    int cur_weapon = -1;

    player_bin& initialize_bin(int bin);

public:
    bool switching_weapons = false;
    int next_weapon;
    bool weap_assigned = false;
    float mount_wait = 0.0f;

    player_inventory(asset_ref<gorc::inventory> BaseInventory);

    player_bin& get_bin(int bin);

    int get_bin_value(int bin);
    void set_bin_value(int bin, int value);
    void mod_bin_value(int bin, int delta);

    float get_bin_cooldown(int bin);
    void set_bin_cooldown(int bin, float value);

    bool is_bin_activated(int bin);
    void set_bin_activated(int bin, bool value);

    bool is_bin_available(int bin);
    void set_bin_available(int bin, bool value);

    inline int get_cur_weapon() const {
        return cur_weapon;
    }

    inline void set_cur_weapon(int weap_bin) {
        cur_weapon = weap_bin;
    }

    inline bool is_cur_weapon_active() {
        return cur_weapon >= 0 && get_bin(cur_weapon).activated;
    }

    inline int get_fire_mode() {
        if(cur_weapon < 0) {
            return -1;
        }

        auto& bin = get_bin(cur_weapon);
        if(bin.activated) {
            return bin.mode;
        }

        return -1;
    }

    void mod_all_cooldowns(float dt);

    auto begin() -> decltype(bins.begin()) {
        return bins.begin();
    }

    auto end() -> decltype(bins.end()) {
        return bins.end();
    }
};

}
}
}
}
