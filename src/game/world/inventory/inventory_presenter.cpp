#include "inventory_presenter.h"
#include "game/world/level_model.h"
#include "inventory_model.h"
#include "cog/verbs/table.h"
#include "game/level_state.h"
#include "game/world/level_presenter.h"
#include "game/world/scripts/script_presenter.h"

gorc::game::world::inventory::inventory_presenter::inventory_presenter(level_presenter& presenter)
    : presenter(presenter) {
    return;
}

void gorc::game::world::inventory::inventory_presenter::start(level_model& levelModel, inventory_model& model) {
    this->model = &model;
    this->levelModel = &levelModel;
    return;
}

void gorc::game::world::inventory::inventory_presenter::update(const time& time) {
    model->mod_all_cooldowns(-time);

    // Update firing messages.
    for(auto& player_inv : *model) {
        for(auto& player_bin : std::get<1>(player_inv)) {
            auto bin_id = std::get<0>(player_bin);
            auto& bin = std::get<1>(player_bin);

            bin.refire_time_activated += time;

            if(bin.activated) {
                bin.total_time_activated += time;
            }

            if(bin.activated && bin.refiring) {
                if(bin.refire_time_activated >= bin.refire_rate) {
                    bin.refire_time_activated = 0.0;
                    presenter.script_presenter->send_message(get_inv_cog(std::get<0>(player_inv), bin_id), cog::message_id::fire,
                            -1, bin.mode, flags::message_type::system,
                            std::get<0>(player_inv), flags::message_type::thing);
                }
            }
        }

        auto& inv = std::get<1>(player_inv);
        inv.mount_wait -= time;
        if(inv.switching_weapons && inv.mount_wait <= 0.0f) {
            inv.switching_weapons = false;
            // TODO: When weapon is ASSIGNED, pass sender ref of 1.
            presenter.script_presenter->send_message(get_inv_cog(std::get<0>(player_inv), inv.next_weapon), cog::message_id::selected,
                    -1, inv.weap_assigned ? 1 : 0, flags::message_type::system,
                    std::get<0>(player_inv), flags::message_type::thing);
        }
    }
}

void gorc::game::world::inventory::inventory_presenter::change_inv(int player, int bin, int amount) {
    model->get_inventory(player).mod_bin_value(bin, amount);
}

int gorc::game::world::inventory::inventory_presenter::get_inv(int player, int bin) {
    return model->get_inventory(player).get_bin_value(bin);
}

int gorc::game::world::inventory::inventory_presenter::get_inv_cog(int player, int bin) {
    const content::assets::inventory_bin& inv_bin = model->base_inventory.get_bin(bin);
    if(inv_bin.cog) {
        return presenter.script_presenter->get_global_cog_instance(&inv_bin.cog->cogscript);
    }
    else {
        return -1;
    }
}

int gorc::game::world::inventory::inventory_presenter::get_inv_max(int player, int bin) {
    return model->base_inventory.get_bin(bin).max_value;
}

int gorc::game::world::inventory::inventory_presenter::get_inv_min(int player, int bin) {
    return model->base_inventory.get_bin(bin).min_value;
}

bool gorc::game::world::inventory::inventory_presenter::is_inv_activated(int player, int bin) {
    return model->get_inventory(player).is_bin_activated(bin);
}

bool gorc::game::world::inventory::inventory_presenter::is_inv_available(int player, int bin) {
    return model->get_inventory(player).is_bin_available(bin);
}

void gorc::game::world::inventory::inventory_presenter::set_bin_wait(int player, int bin, float delay) {
    model->get_inventory(player).set_bin_cooldown(bin, delay);
}

void gorc::game::world::inventory::inventory_presenter::set_inv(int player, int bin, int value) {
    model->get_inventory(player).set_bin_value(bin, value);
}

void gorc::game::world::inventory::inventory_presenter::set_inv_activated(int player, int bin, bool value) {
    model->get_inventory(player).set_bin_activated(bin, value);
}

void gorc::game::world::inventory::inventory_presenter::set_inv_available(int player, int bin, bool value) {
    model->get_inventory(player).set_bin_available(bin, value);
}

void gorc::game::world::inventory::inventory_presenter::assign_weapon(int player, int weapon_bin) {
    select_weapon(player, weapon_bin);

    auto& player_inv = model->get_inventory(player);
    if(player_inv.switching_weapons) {
        player_inv.weap_assigned = true;
    }
}

void gorc::game::world::inventory::inventory_presenter::activate_weapon(int player, float firewait, int mode) {
    int cur_weapon = get_cur_weapon(player);

    if(cur_weapon >= 0) {
        auto& bin = model->get_inventory(player).get_bin(cur_weapon);
        bin.activated = true;
        bin.refiring = firewait > 0.0f;
        bin.total_time_activated = 0.0f;
        bin.mode = mode;
        bin.refire_rate = firewait;
    }
}

int gorc::game::world::inventory::inventory_presenter::autoselect_weapon(int player, flags::autoselect_mode select_mode) {
    int best_bin = -1;
    int best_bin_value = std::numeric_limits<int>::lowest();

    for(const auto& bin_pair : model->get_inventory(player)) {
        auto& base_bin = model->base_inventory.get_bin(std::get<0>(bin_pair));
        if(base_bin.flags & flags::inventory_flag::weapon) {
            int auto_res = presenter.script_presenter->send_message(get_inv_cog(player, std::get<0>(bin_pair)), cog::message_id::autoselect,
                    -1, static_cast<int>(flags::autoselect_mode::player_mounting), flags::message_type::system,
                    player, flags::message_type::thing);
            if(auto_res >= best_bin) {
                best_bin = auto_res;
                best_bin_value = std::get<0>(bin_pair);
            }
        }
    }

    return best_bin_value;
}

void gorc::game::world::inventory::inventory_presenter::change_fire_rate(int player, float firewait) {
    int cur_weapon = get_cur_weapon(player);
    if(cur_weapon >= 0) {
        auto& bin = model->get_inventory(player).get_bin(cur_weapon);
        bin.refire_rate = firewait;
    }
}

float gorc::game::world::inventory::inventory_presenter::deactivate_weapon(int player, int fire_mode) {
    int cur_weapon = get_cur_weapon(player);

    if(cur_weapon >= 0) {
        auto& bin = model->get_inventory(player).get_bin(cur_weapon);
        bin.activated = false;
        return bin.total_time_activated;
    }

    return 0.0f;
}

int gorc::game::world::inventory::inventory_presenter::get_cur_weapon(int player) {
    return model->get_inventory(player).get_cur_weapon();
}

int gorc::game::world::inventory::inventory_presenter::get_cur_weapon_mode() {
    int player = presenter.get_local_player_thing();
    int cur_weapon = get_cur_weapon(player);

    if(cur_weapon >= 0) {
        const auto& weap_bin = model->get_inventory(player).get_bin(cur_weapon);
        if(weap_bin.activated && weap_bin.refiring) {
            return weap_bin.mode;
        }
    }

    return -1;
}

void gorc::game::world::inventory::inventory_presenter::select_weapon(int player, int weap_bin) {
    if(weap_bin == get_cur_weapon(player)) {
        // Already selected. Skip the rest.
        return;
    }

    auto cog = get_inv_cog(player, weap_bin);
    int auto_res = presenter.script_presenter->send_message(get_inv_cog(player, weap_bin), cog::message_id::autoselect,
            -1, static_cast<int>(flags::autoselect_mode::player_mounting), flags::message_type::system,
            player, flags::message_type::thing);

    if(auto_res > -1) {
        // Can mount the weapon
        auto& player_inv = model->get_inventory(player);

        // Check to see if already switching away from current weapon.
        if(!player_inv.switching_weapons) {
            // Deactivate weapon.
            if(player_inv.is_cur_weapon_active()) {
                deactivate_weapon(player, player_inv.get_fire_mode());
            }

            // Deselect weapon
            if(player_inv.get_cur_weapon() >= 0) {
                presenter.script_presenter->send_message(get_inv_cog(player, player_inv.get_cur_weapon()), cog::message_id::deselected,
                        -1, 0, flags::message_type::system,
                        player, flags::message_type::thing);
            }
        }

        player_inv.next_weapon = weap_bin;
        player_inv.switching_weapons = true;
        player_inv.weap_assigned = false;
    }
}

void gorc::game::world::inventory::inventory_presenter::set_cur_inv_weapon(int player, int bin_num) {
    model->get_inventory(player).set_cur_weapon(bin_num);
}

void gorc::game::world::inventory::inventory_presenter::set_cur_weapon(int player, int weap_num) {
    // Convert weapon num into bin num.
    int seen_weap = 0;
    for(const auto& bin : model->base_inventory) {
        if(std::get<1>(bin).flags & flags::inventory_flag::weapon) {
            if(seen_weap == weap_num) {
                set_cur_inv_weapon(player, std::get<0>(bin));
                return;
            }
            else {
                ++seen_weap;
            }
        }
    }
}

void gorc::game::world::inventory::inventory_presenter::set_mount_wait(int player, float delay) {
    model->get_inventory(player).mount_wait = delay;
}

void gorc::game::world::inventory::inventory_presenter::on_item_hotkey_pressed(int player, int bin) {
    presenter.script_presenter->send_message(get_inv_cog(player, bin), cog::message_id::activated,
            -1, bin, flags::message_type::system,
            player, flags::message_type::thing);
}

void gorc::game::world::inventory::inventory_presenter::on_item_hotkey_released(int player, int bin) {
    presenter.script_presenter->send_message(get_inv_cog(player, bin), cog::message_id::deactivated,
            -1, bin, flags::message_type::system,
            player, flags::message_type::thing);
}

void gorc::game::world::inventory::inventory_presenter::on_weapon_fire_pressed(int player, int mode) {
    if(get_cur_weapon(player) >= 0) {
        presenter.script_presenter->send_message(get_inv_cog(player, get_cur_weapon(player)), cog::message_id::activated,
                -1, mode, flags::message_type::system,
                player, flags::message_type::thing);
    }
}

void gorc::game::world::inventory::inventory_presenter::on_weapon_fire_released(int player, int mode) {
    if(get_cur_weapon(player) >= 0) {
        presenter.script_presenter->send_message(get_inv_cog(player, get_cur_weapon(player)), cog::message_id::deactivated,
                -1, mode, flags::message_type::system,
                player, flags::message_type::thing);
    }
}

void gorc::game::world::inventory::inventory_presenter::clear_goal_flags(int player, int goal, flag_set<flags::goal_flag> flags) {
    set_inv(player, 100 + goal, static_cast<int>(flag_set<flags::goal_flag>(get_inv(player, 100 + goal)) - flags));
}

void gorc::game::world::inventory::inventory_presenter::set_goal_flags(int player, int goal, flag_set<flags::goal_flag> flags) {
    set_inv(player, 100 + goal, static_cast<int>(flag_set<flags::goal_flag>(get_inv(player, 100 + goal)) + flags));
}

void gorc::game::world::inventory::inventory_presenter::register_verbs(cog::verbs::verb_table& verbTable, level_state& components) {
    verbTable.add_verb<void, 3>("changeinv", [&components](int player, int bin, int amount) {
        components.current_level_presenter->inventory_presenter->change_inv(player, bin, amount);
    });

    verbTable.add_verb<int, 2>("getinv", [&components](int player, int bin) {
        return components.current_level_presenter->inventory_presenter->get_inv(player, bin);
    });

    verbTable.add_verb<int, 2>("getinvmax", [&components](int player, int bin) {
        return components.current_level_presenter->inventory_presenter->get_inv_max(player, bin);
    });

    verbTable.add_verb<int, 2>("getinvmin", [&components](int player, int bin) {
        return components.current_level_presenter->inventory_presenter->get_inv_min(player, bin);
    });

    verbTable.add_verb<bool, 2>("isinvactivated", [&components](int player, int bin) {
        return components.current_level_presenter->inventory_presenter->is_inv_activated(player, bin);
    });

    verbTable.add_verb<bool, 2>("isinvavailable", [&components](int player, int bin) {
        return components.current_level_presenter->inventory_presenter->is_inv_available(player, bin);
    });

    verbTable.add_verb<void, 3>("setbinwait", [&components](int player, int bin, float value) {
        components.current_level_presenter->inventory_presenter->set_bin_wait(player, bin, value);
    });

    verbTable.add_verb<void, 3>("setinv", [&components](int player, int bin, int value) {
        components.current_level_presenter->inventory_presenter->set_inv(player, bin, value);
    });

    verbTable.add_verb<void, 3>("setinvactivated", [&components](int player, int bin, bool value) {
        components.current_level_presenter->inventory_presenter->set_inv_activated(player, bin, value);
    });

    verbTable.add_verb<void, 3>("setinvavailable", [&components](int player, int bin, bool value) {
        components.current_level_presenter->inventory_presenter->set_inv_available(player, bin, value);
    });

    // Weapon verbs
    verbTable.add_verb<void, 2>("assignweapon", [&components](int player, int weap_bin) {
        components.current_level_presenter->inventory_presenter->assign_weapon(player, weap_bin);
    });

    verbTable.add_verb<void, 3>("activateweapon", [&components](int player, float firewait, int mode) {
        components.current_level_presenter->inventory_presenter->activate_weapon(player, firewait, mode);
    });

    verbTable.add_verb<int, 2>("autoselectweapon", [&components](int player, int select_mode) {
        return components.current_level_presenter->inventory_presenter->autoselect_weapon(player, static_cast<flags::autoselect_mode>(select_mode));
    });

    verbTable.add_verb<void, 2>("changefirerate", [&components](int player, float firewait) {
        components.current_level_presenter->inventory_presenter->change_fire_rate(player, firewait);
    });

    verbTable.add_verb<float, 2>("deactivateweapon", [&components](int player, int mode) {
        return components.current_level_presenter->inventory_presenter->deactivate_weapon(player, mode);
    });

    verbTable.add_verb<int, 1>("getcurinvweapon", [&components](int player) {
        return components.current_level_presenter->inventory_presenter->get_cur_weapon(player);
    });

    verbTable.add_verb<int, 1>("getcurweapon", [&components](int player) {
        return components.current_level_presenter->inventory_presenter->get_cur_weapon(player);
    });

    verbTable.add_verb<int, 0>("getcurweaponmode", [&components] {
        return components.current_level_presenter->inventory_presenter->get_cur_weapon_mode();
    });

    verbTable.add_verb<void, 2>("selectweapon", [&components](int player, int weap_bin) {
        components.current_level_presenter->inventory_presenter->select_weapon(player, weap_bin);
    });

    verbTable.add_verb<void, 2>("setcurinvweapon", [&components](int player, int weap_bin) {
        components.current_level_presenter->inventory_presenter->set_cur_inv_weapon(player, weap_bin);
    });

    verbTable.add_verb<void, 2>("setcurweapon", [&components](int player, int weap_num) {
        components.current_level_presenter->inventory_presenter->set_cur_weapon(player, weap_num);
    });

    verbTable.add_verb<void, 2>("setfirewait", [&components](int player, float wait) {
        // TODO
    });

    verbTable.add_verb<void, 2>("setmountwait", [&components](int player, float delay) {
        components.current_level_presenter->inventory_presenter->set_mount_wait(player, delay);
    });

    verbTable.add_verb<void, 3>("cleargoalflags", [&components](int player, int goal, int flags) {
        components.current_level_presenter->inventory_presenter->clear_goal_flags(player, goal, flag_set<flags::goal_flag>(flags));
    });

    verbTable.add_verb<void, 3>("setgoalflags", [&components](int player, int goal, int flags) {
        components.current_level_presenter->inventory_presenter->set_goal_flags(player, goal, flag_set<flags::goal_flag>(flags));
    });
}
