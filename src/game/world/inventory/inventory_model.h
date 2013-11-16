#pragma once

#include <unordered_map>
#include "content/assets/inventory.h"

namespace gorc {
namespace game {
namespace world {
namespace inventory {

class player_bin_model {
public:
	bool activated = false;
	bool available = false;
	int value = 0;
	float cooldown = 0.0f;
};

class player_inventory_model {
private:
	const content::assets::inventory& BaseInventory;
	std::unordered_map<int, player_bin_model> bins;

	player_bin_model& initialize_bin(int bin);
	player_bin_model& get_bin(int bin);

public:
	player_inventory_model(const content::assets::inventory& BaseInventory);

	int get_bin_value(int bin);
	void set_bin_value(int bin, int value);
	void mod_bin_value(int bin, int delta);

	float get_bin_cooldown(int bin);
	void set_bin_cooldown(int bin, float value);

	bool is_bin_activated(int bin);
	void set_bin_activated(int bin, bool value);

	bool is_bin_available(int bin);
	void set_bin_available(int bin, bool value);

	void mod_all_cooldowns(float dt);
};

class inventory_model {
private:
	std::unordered_map<int, player_inventory_model> player_inventories;

public:
	const content::assets::inventory& base_inventory;

	inventory_model(const content::assets::inventory& BaseInventory);

	player_inventory_model& get_inventory(int player_id);
	void mod_all_cooldowns(float dt);
};

}
}
}
}
