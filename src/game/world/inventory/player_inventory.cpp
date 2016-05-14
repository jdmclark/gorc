#include "player_inventory.hpp"

using namespace gorc::game::world::inventory;

player_inventory::player_inventory(asset_ref<gorc::inventory> BaseInventory)
    : BaseInventory(BaseInventory) {
    return;
}

player_bin& player_inventory::initialize_bin(int bin) {
    const auto& base_bin = BaseInventory->get_bin(bin);

    auto& new_bin = std::get<1>(*std::get<0>(bins.emplace(bin, player_bin())));

    new_bin.cooldown = 0.0f;
    new_bin.value = base_bin.min_value;
    new_bin.available = base_bin.flags & inventory_flag::available_by_default;
    return new_bin;
}

player_bin& player_inventory::get_bin(int bin) {
    auto it = bins.find(bin);
    if(it == bins.end()) {
        return initialize_bin(bin);
    }
    else {
        return it->second;
    }
}

int player_inventory::get_bin_value(int bin) {
    return get_bin(bin).value;
}

void player_inventory::set_bin_value(int bin, int value) {
    const auto& base_bin = BaseInventory->get_bin(bin);
    get_bin(bin).value = std::min(std::max(value, base_bin.min_value), base_bin.max_value);
}

void player_inventory::mod_bin_value(int bin, int delta) {
    set_bin_value(bin, get_bin(bin).value + delta);
}

float player_inventory::get_bin_cooldown(int bin) {
    return get_bin(bin).cooldown;
}

void player_inventory::set_bin_cooldown(int bin, float value) {
    get_bin(bin).cooldown = value;
}

bool player_inventory::is_bin_activated(int bin) {
    return get_bin(bin).activated;
}

void player_inventory::set_bin_activated(int bin, bool state) {
    get_bin(bin).activated = state;
}

bool player_inventory::is_bin_available(int bin) {
    return get_bin(bin).available;
}

void player_inventory::set_bin_available(int bin, bool state) {
    get_bin(bin).available = state;
}

void player_inventory::mod_all_cooldowns(float value) {
    for(auto& bin_pair : bins) {
        std::get<1>(bin_pair).cooldown = std::max(std::get<1>(bin_pair).cooldown + value, 0.0f);
    }
}
