#include "inventory_presenter.h"
#include "game/world/level_model.h"
#include "inventory_model.h"
#include "cog/verbs/table.h"
#include "game/application.h"
#include "game/world/level_presenter.h"

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
		return presenter.script_presenter.get_global_cog_instance(&inv_bin.cog->cogscript);
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

void gorc::game::world::inventory::inventory_presenter::on_item_hotkey_pressed(int player, int bin) {
	// TODO: If weapon, change 'bin' to 0 for primary, 1 for secondary fire.
	presenter.script_presenter.send_message(get_inv_cog(player, bin), cog::message_id::activated,
			-1, bin, flags::message_type::system,
			player, flags::message_type::thing);
}

void gorc::game::world::inventory::inventory_presenter::on_item_hotkey_released(int player, int bin) {
	// TODO: If weapon, change 'bin' to 0 for primary, 1 for secondary fire.
	presenter.script_presenter.send_message(get_inv_cog(player, bin), cog::message_id::deactivated,
			-1, bin, flags::message_type::system,
			player, flags::message_type::thing);
}

void gorc::game::world::inventory::inventory_presenter::register_verbs(cog::verbs::verb_table& verbTable, application& components) {
	verbTable.add_verb<void, 3>("changeinv", [&components](int player, int bin, int amount) {
		components.current_level_presenter->inventory_presenter.change_inv(player, bin, amount);
	});

	verbTable.add_verb<int, 2>("getinv", [&components](int player, int bin) {
		return components.current_level_presenter->inventory_presenter.get_inv(player, bin);
	});

	verbTable.add_verb<int, 2>("getinvmax", [&components](int player, int bin) {
		return components.current_level_presenter->inventory_presenter.get_inv_max(player, bin);
	});

	verbTable.add_verb<int, 2>("getinvmin", [&components](int player, int bin) {
		return components.current_level_presenter->inventory_presenter.get_inv_min(player, bin);
	});

	verbTable.add_verb<bool, 2>("isinvactivated", [&components](int player, int bin) {
		return components.current_level_presenter->inventory_presenter.is_inv_activated(player, bin);
	});

	verbTable.add_verb<bool, 2>("isinvavailable", [&components](int player, int bin) {
		return components.current_level_presenter->inventory_presenter.is_inv_available(player, bin);
	});

	verbTable.add_verb<void, 3>("setbinwait", [&components](int player, int bin, float value) {
		components.current_level_presenter->inventory_presenter.set_bin_wait(player, bin, value);
	});

	verbTable.add_verb<void, 3>("setinv", [&components](int player, int bin, int value) {
		components.current_level_presenter->inventory_presenter.set_inv(player, bin, value);
	});

	verbTable.add_verb<void, 3>("setinvactivated", [&components](int player, int bin, bool value) {
		components.current_level_presenter->inventory_presenter.set_inv_activated(player, bin, value);
	});

	verbTable.add_verb<void, 3>("setinvavailable", [&components](int player, int bin, bool value) {
		components.current_level_presenter->inventory_presenter.set_inv_available(player, bin, value);
	});
}
