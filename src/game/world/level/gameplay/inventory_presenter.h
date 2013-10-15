#pragma once

namespace gorc {
namespace cog {
namespace verbs {
class verb_table;
}
}

namespace game {
class components;

namespace world {
namespace level {
class level_model;
class level_presenter;

namespace gameplay {

class inventory_model;

class inventory_presenter {
private:
	level_presenter& presenter;
	inventory_model* model;
	level_model* levelModel;

public:
	inventory_presenter(level_presenter& presenter);

	void start(level_model& levelModel, inventory_model& model);
	void update(double dt);

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

	void on_item_hotkey_pressed(int player, int bin);
	void on_item_hotkey_released(int player, int bin);

	static void register_verbs(cog::verbs::verb_table& verbTable, components&);
};

}
}
}
}
}
