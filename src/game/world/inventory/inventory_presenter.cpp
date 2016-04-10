#include "inventory_presenter.hpp"
#include "game/world/level_model.hpp"
#include "inventory_model.hpp"
#include "game/level_state.hpp"
#include "game/world/level_presenter.hpp"
#include <limits>

gorc::game::world::inventory::inventory_presenter::inventory_presenter(level_presenter& presenter)
    : presenter(presenter) {
    return;
}

void gorc::game::world::inventory::inventory_presenter::start(level_model& levelModel, inventory_model& model) {
    this->model = &model;
    this->levelModel = &levelModel;
    return;
}

void gorc::game::world::inventory::inventory_presenter::update(const gorc::time& time) {
    model->mod_all_cooldowns(-static_cast<float>(time.elapsed_as_seconds()));

    // Update firing messages.
    for(auto& player_inv : *model) {
        for(auto& player_bin : std::get<1>(player_inv)) {
            auto bin_id = std::get<0>(player_bin);
            auto& bin = std::get<1>(player_bin);

            bin.refire_time_activated += static_cast<float>(time.elapsed_as_seconds());

            if(bin.activated) {
                bin.total_time_activated += static_cast<float>(time.elapsed_as_seconds());
            }

            if(bin.activated && bin.refiring) {
                if(bin.refire_time_activated >= bin.refire_rate) {
                    bin.refire_time_activated = 0.0f;
                    maybe_if(get_inv_cog(std::get<0>(player_inv), bin_id), [&](auto bin_cog) {
                        levelModel->script_model.send(bin_cog,
                                                      cog::message_type::fire,
                                                      /* senderref */ bin.mode,
                                                      /* senderid */ cog::value(),
                                                      /* sourceref */ std::get<0>(player_inv));
                    });
                }
            }
        }

        auto& inv = std::get<1>(player_inv);
        inv.mount_wait -= static_cast<float>(time.elapsed_as_seconds());
        if(inv.switching_weapons && inv.mount_wait <= 0.0f) {
            inv.switching_weapons = false;
            // Pass senderref of 1 when weapon is ASSIGNED, 0 when SELECTED.
            maybe_if(get_inv_cog(std::get<0>(player_inv), inv.next_weapon), [&](auto bin_cog) {
                levelModel->script_model.send(bin_cog,
                                              cog::message_type::selected,
                                              /* senderref */ (inv.weap_assigned ? 1 : 0),
                                              /* senderid */ cog::value(),
                                              /* sourceref */ std::get<0>(player_inv));
            });
        }
    }
}

void gorc::game::world::inventory::inventory_presenter::change_inv(thing_id player, int bin, int amount) {
    model->get_inventory(player).mod_bin_value(bin, amount);
}

int gorc::game::world::inventory::inventory_presenter::get_inv(thing_id player, int bin) {
    return model->get_inventory(player).get_bin_value(bin);
}

gorc::maybe<gorc::cog_id> gorc::game::world::inventory::inventory_presenter::get_inv_cog(thing_id, int bin) {
    const content::assets::inventory_bin& inv_bin = model->base_inventory->get_bin(bin);
    return maybe_if(inv_bin.cog, maybe<cog_id>(), [&](auto cog) {
        return levelModel->script_model.create_global_instance(cog);
    });
}

int gorc::game::world::inventory::inventory_presenter::get_inv_max(thing_id, int bin) {
    return model->base_inventory->get_bin(bin).max_value;
}

int gorc::game::world::inventory::inventory_presenter::get_inv_min(thing_id, int bin) {
    return model->base_inventory->get_bin(bin).min_value;
}

bool gorc::game::world::inventory::inventory_presenter::is_inv_activated(thing_id player, int bin) {
    return model->get_inventory(player).is_bin_activated(bin);
}

bool gorc::game::world::inventory::inventory_presenter::is_inv_available(thing_id player, int bin) {
    return model->get_inventory(player).is_bin_available(bin);
}

void gorc::game::world::inventory::inventory_presenter::set_bin_wait(thing_id player, int bin, float delay) {
    model->get_inventory(player).set_bin_cooldown(bin, delay);
}

void gorc::game::world::inventory::inventory_presenter::set_inv(thing_id player, int bin, int value) {
    model->get_inventory(player).set_bin_value(bin, value);
}

void gorc::game::world::inventory::inventory_presenter::set_inv_activated(thing_id player, int bin, bool value) {
    model->get_inventory(player).set_bin_activated(bin, value);
}

void gorc::game::world::inventory::inventory_presenter::set_inv_available(thing_id player, int bin, bool value) {
    model->get_inventory(player).set_bin_available(bin, value);
}

void gorc::game::world::inventory::inventory_presenter::assign_weapon(thing_id player, int weapon_bin) {
    select_weapon(player, weapon_bin);

    auto& player_inv = model->get_inventory(player);
    if(player_inv.switching_weapons) {
        player_inv.weap_assigned = true;
    }
}

void gorc::game::world::inventory::inventory_presenter::activate_weapon(thing_id player, float firewait, int mode) {
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

int gorc::game::world::inventory::inventory_presenter::autoselect_weapon(thing_id player, flags::autoselect_mode) {
    // TODO: Handle autoselect mode
    int best_bin = std::numeric_limits<int>::lowest();
    int best_bin_value = -1;

    for(const auto& bin_pair : model->get_inventory(player)) {
        auto& base_bin = model->base_inventory->get_bin(std::get<0>(bin_pair));
        if(base_bin.flags & flags::inventory_flag::weapon) {
            int senderref = static_cast<int>(flags::autoselect_mode::player_mounting);
            int auto_res = std::numeric_limits<int>::lowest();
            maybe_if(get_inv_cog(player, std::get<0>(bin_pair)), [&](auto bin_cog) {
                auto_res = levelModel->script_model.send(bin_cog,
                                                         cog::message_type::autoselect,
                                                         senderref,
                                                         /* senderid */ cog::value(),
                                                         /* sourceref */ player);
            });

            if(auto_res >= best_bin) {
                best_bin = auto_res;
                best_bin_value = std::get<0>(bin_pair);
            }
        }
    }

    return best_bin_value;
}

void gorc::game::world::inventory::inventory_presenter::change_fire_rate(thing_id player, float firewait) {
    int cur_weapon = get_cur_weapon(player);
    if(cur_weapon >= 0) {
        auto& bin = model->get_inventory(player).get_bin(cur_weapon);
        bin.refire_rate = firewait;
    }
}

float gorc::game::world::inventory::inventory_presenter::deactivate_weapon(thing_id player, int) {
    // TODO: Handle fire mode
    int cur_weapon = get_cur_weapon(player);

    if(cur_weapon >= 0) {
        auto& bin = model->get_inventory(player).get_bin(cur_weapon);
        bin.activated = false;
        return bin.total_time_activated;
    }

    return 0.0f;
}

int gorc::game::world::inventory::inventory_presenter::get_cur_weapon(thing_id player) {
    return model->get_inventory(player).get_cur_weapon();
}

int gorc::game::world::inventory::inventory_presenter::get_cur_weapon_mode() {
    thing_id player = thing_id(presenter.get_local_player_thing());
    int cur_weapon = get_cur_weapon(player);

    if(cur_weapon >= 0) {
        const auto& weap_bin = model->get_inventory(player).get_bin(cur_weapon);
        if(weap_bin.activated && weap_bin.refiring) {
            return weap_bin.mode;
        }
    }

    return -1;
}

void gorc::game::world::inventory::inventory_presenter::select_weapon(thing_id player, int weap_bin) {
    if(weap_bin == get_cur_weapon(player)) {
        // Already selected. Skip the rest.
        return;
    }

    int senderref = static_cast<int>(flags::autoselect_mode::player_mounting);
    int auto_res = -1;
    maybe_if(get_inv_cog(player, weap_bin), [&](auto bin_cog) {
        auto_res = levelModel->script_model.send(bin_cog,
                                                 cog::message_type::autoselect,
                                                 senderref,
                                                 /* senderid */ cog::value(),
                                                 /* sourceref */ player);
    });

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
                maybe_if(get_inv_cog(player, player_inv.get_cur_weapon()), [&](auto bin_cog) {
                    levelModel->script_model.send(bin_cog,
                                                  cog::message_type::deselected,
                                                  /* senderref */ cog::value(),
                                                  /* senderid */ cog::value(),
                                                  /* sourceref */ player);
                });
            }
        }

        player_inv.next_weapon = weap_bin;
        player_inv.switching_weapons = true;
        player_inv.weap_assigned = false;
    }
}

void gorc::game::world::inventory::inventory_presenter::set_cur_inv_weapon(thing_id player, int bin_num) {
    model->get_inventory(player).set_cur_weapon(bin_num);
}

void gorc::game::world::inventory::inventory_presenter::set_cur_weapon(thing_id player, int weap_num) {
    // Convert weapon num into bin num.
    int seen_weap = 0;
    for(const auto& bin : *model->base_inventory) {
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

void gorc::game::world::inventory::inventory_presenter::set_mount_wait(thing_id player, float delay) {
    model->get_inventory(player).mount_wait = delay;
}

void gorc::game::world::inventory::inventory_presenter::on_item_hotkey_pressed(thing_id player, int bin) {
    maybe_if(get_inv_cog(player, bin), [&](auto bin_cog) {
        levelModel->script_model.send(bin_cog,
                                      cog::message_type::activated,
                                      /* senderref */ bin,
                                      /* senderid */ cog::value(),
                                      /* sourceref */ player);
    });
}

void gorc::game::world::inventory::inventory_presenter::on_item_hotkey_released(thing_id player, int bin) {
    maybe_if(get_inv_cog(player, bin), [&](auto bin_cog) {
        levelModel->script_model.send(bin_cog,
                                      cog::message_type::deactivated,
                                      /* senderref */ bin,
                                      /* senderid */ cog::value(),
                                      /* sourceref */ player);
    });
}

void gorc::game::world::inventory::inventory_presenter::on_weapon_fire_pressed(thing_id player, int mode) {
    if(get_cur_weapon(player) >= 0) {
        maybe_if(get_inv_cog(player, get_cur_weapon(player)), [&](auto bin_cog) {
            levelModel->script_model.send(bin_cog,
                                          cog::message_type::activated,
                                          /* senderref */ mode,
                                          /* senderid */ cog::value(),
                                          /* sourceref */ player);
        });
    }
}

void gorc::game::world::inventory::inventory_presenter::on_weapon_fire_released(thing_id player, int mode) {
    if(get_cur_weapon(player) >= 0) {
        maybe_if(get_inv_cog(player, get_cur_weapon(player)), [&](auto bin_cog) {
            levelModel->script_model.send(bin_cog,
                                          cog::message_type::deactivated,
                                          /* senderref */ mode,
                                          /* senderid */ cog::value(),
                                          /* sourceref */ player);
        });
    }
}

void gorc::game::world::inventory::inventory_presenter::clear_goal_flags(thing_id player, int goal, flag_set<flags::goal_flag> flags) {
    set_inv(player, 100 + goal, static_cast<int>(flag_set<flags::goal_flag>(get_inv(player, 100 + goal)) - flags));
}

void gorc::game::world::inventory::inventory_presenter::set_goal_flags(thing_id player, int goal, flag_set<flags::goal_flag> flags) {
    set_inv(player, 100 + goal, static_cast<int>(flag_set<flags::goal_flag>(get_inv(player, 100 + goal)) + flags));
}

void gorc::game::world::inventory::inventory_presenter::register_verbs(cog::verb_table &verbs, level_state &components) {
    verbs.add_verb("changeinv", [&components](thing_id player, int bin, int amount) {
        components.current_level_presenter->inventory_presenter->change_inv(player, bin, amount);
    });

    verbs.add_verb("getinv", [&components](thing_id player, int bin) {
        return components.current_level_presenter->inventory_presenter->get_inv(player, bin);
    });

    verbs.add_verb("getinvmax", [&components](thing_id player, int bin) {
        return components.current_level_presenter->inventory_presenter->get_inv_max(player, bin);
    });

    verbs.add_verb("getinvmin", [&components](thing_id player, int bin) {
        return components.current_level_presenter->inventory_presenter->get_inv_min(player, bin);
    });

    verbs.add_verb("isinvactivated", [&components](thing_id player, int bin) {
        return components.current_level_presenter->inventory_presenter->is_inv_activated(player, bin);
    });

    verbs.add_verb("isinvavailable", [&components](thing_id player, int bin) {
        return components.current_level_presenter->inventory_presenter->is_inv_available(player, bin);
    });

    verbs.add_verb("setbinwait", [&components](thing_id player, int bin, float value) {
        components.current_level_presenter->inventory_presenter->set_bin_wait(player, bin, value);
    });

    verbs.add_verb("setinv", [&components](thing_id player, int bin, int value) {
        components.current_level_presenter->inventory_presenter->set_inv(player, bin, value);
    });

    verbs.add_verb("setinvactivated", [&components](thing_id player, int bin, int value) {
        components.current_level_presenter->inventory_presenter->set_inv_activated(player, bin, value);
    });

    verbs.add_verb("setinvavailable", [&components](thing_id player, int bin, int value) {
        components.current_level_presenter->inventory_presenter->set_inv_available(player, bin, value);
    });

    // Weapon verbs
    verbs.add_verb("assignweapon", [&components](thing_id player, int weap_bin) {
        components.current_level_presenter->inventory_presenter->assign_weapon(player, weap_bin);
    });

    verbs.add_verb("activateweapon", [&components](thing_id player, float firewait, int mode) {
        components.current_level_presenter->inventory_presenter->activate_weapon(player, firewait, mode);
    });

    verbs.add_verb("autoselectweapon", [&components](thing_id player, int select_mode) {
        return components.current_level_presenter->inventory_presenter->autoselect_weapon(player, static_cast<flags::autoselect_mode>(select_mode));
    });

    verbs.add_verb("changefirerate", [&components](thing_id player, float firewait) {
        components.current_level_presenter->inventory_presenter->change_fire_rate(player, firewait);
    });

    verbs.add_verb("deactivateweapon", [&components](thing_id player, int mode) {
        return components.current_level_presenter->inventory_presenter->deactivate_weapon(player, mode);
    });

    verbs.add_verb("getcurinvweapon", [&components](thing_id player) {
        return components.current_level_presenter->inventory_presenter->get_cur_weapon(player);
    });

    verbs.add_verb("getcurweapon", [&components](thing_id player) {
        return components.current_level_presenter->inventory_presenter->get_cur_weapon(player);
    });

    verbs.add_verb("getcurweaponmode", [&components] {
        return components.current_level_presenter->inventory_presenter->get_cur_weapon_mode();
    });

    verbs.add_verb("selectweapon", [&components](thing_id player, int weap_bin) {
        components.current_level_presenter->inventory_presenter->select_weapon(player, weap_bin);
    });

    verbs.add_verb("setcurinvweapon", [&components](thing_id player, int weap_bin) {
        components.current_level_presenter->inventory_presenter->set_cur_inv_weapon(player, weap_bin);
    });

    verbs.add_verb("setcurweapon", [&components](thing_id player, int weap_num) {
        components.current_level_presenter->inventory_presenter->set_cur_weapon(player, weap_num);
    });

    verbs.add_verb("setfirewait", [&components](thing_id, float) {
        // TODO
    });

    verbs.add_verb("setmountwait", [&components](thing_id player, float delay) {
        components.current_level_presenter->inventory_presenter->set_mount_wait(player, delay);
    });

    verbs.add_verb("cleargoalflags", [&components](thing_id player, int goal, int flags) {
        components.current_level_presenter->inventory_presenter->clear_goal_flags(player, goal, flag_set<flags::goal_flag>(flags));
    });

    verbs.add_verb("setgoalflags", [&components](thing_id player, int goal, int flags) {
        components.current_level_presenter->inventory_presenter->set_goal_flags(player, goal, flag_set<flags::goal_flag>(flags));
    });
}
