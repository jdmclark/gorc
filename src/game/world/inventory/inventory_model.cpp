#include "inventory_model.hpp"

using namespace gorc::game::world::inventory;

player_inventory_model::player_inventory_model(asset_ref<content::assets::inventory> BaseInventory)
    : BaseInventory(BaseInventory) {
    return;
}

player_bin_model& player_inventory_model::initialize_bin(int bin) {
    const auto& base_bin = BaseInventory->get_bin(bin);

    auto& new_bin = std::get<1>(*std::get<0>(bins.emplace(bin, player_bin_model())));

    new_bin.cooldown = 0.0f;
    new_bin.value = base_bin.min_value;
    new_bin.available = base_bin.flags & flags::inventory_flag::AvailableByDefault;
    return new_bin;
}

player_bin_model& player_inventory_model::get_bin(int bin) {
    auto it = bins.find(bin);
    if(it == bins.end()) {
        return initialize_bin(bin);
    }
    else {
        return it->second;
    }
}

int player_inventory_model::get_bin_value(int bin) {
    return get_bin(bin).value;
}

void player_inventory_model::set_bin_value(int bin, int value) {
    const auto& base_bin = BaseInventory->get_bin(bin);
    get_bin(bin).value = std::min(std::max(value, base_bin.min_value), base_bin.max_value);
}

void player_inventory_model::mod_bin_value(int bin, int delta) {
    set_bin_value(bin, get_bin(bin).value + delta);
}

float player_inventory_model::get_bin_cooldown(int bin) {
    return get_bin(bin).cooldown;
}

void player_inventory_model::set_bin_cooldown(int bin, float value) {
    get_bin(bin).cooldown = value;
}

bool player_inventory_model::is_bin_activated(int bin) {
    return get_bin(bin).activated;
}

void player_inventory_model::set_bin_activated(int bin, bool state) {
    get_bin(bin).activated = state;
}

bool player_inventory_model::is_bin_available(int bin) {
    return get_bin(bin).available;
}

void player_inventory_model::set_bin_available(int bin, bool state) {
    get_bin(bin).available = state;
}

void player_inventory_model::mod_all_cooldowns(float value) {
    for(auto& bin_pair : bins) {
        std::get<1>(bin_pair).cooldown = std::max(std::get<1>(bin_pair).cooldown + value, 0.0f);
    }
}

inventory_model::inventory_model(asset_ref<content::assets::inventory> BaseInventory)
    : base_inventory(BaseInventory) {
    return;
}

player_inventory_model& inventory_model::get_inventory(thing_id player_id) {
    auto it = player_inventories.find(player_id);
    if(it == player_inventories.end()) {
        return std::get<0>(player_inventories.emplace(player_id, player_inventory_model(base_inventory)))->second;
    }
    else {
        return it->second;
    }
}

void inventory_model::mod_all_cooldowns(float dt) {
    for(auto& inv_pair : player_inventories) {
        std::get<1>(inv_pair).mod_all_cooldowns(dt);
    }
}
