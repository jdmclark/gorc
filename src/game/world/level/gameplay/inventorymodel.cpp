#include "inventorymodel.h"

using namespace Gorc::Game::World::Level::Gameplay;

PlayerInventoryModel::PlayerInventoryModel(const Content::Assets::Inventory& BaseInventory)
	: BaseInventory(BaseInventory) {
	return;
}

PlayerBinModel& PlayerInventoryModel::InitializeBin(int bin) {
	const auto& base_bin = BaseInventory.GetBin(bin);

	auto& new_bin = std::get<1>(*std::get<0>(bins.emplace(bin, PlayerBinModel())));

	new_bin.Cooldown = 0.0f;
	new_bin.Value = base_bin.MinValue;
	new_bin.Available = base_bin.Flags & Flags::InventoryFlag::AvailableByDefault;
	return new_bin;
}

PlayerBinModel& PlayerInventoryModel::GetBin(int bin) {
	auto it = bins.find(bin);
	if(it == bins.end()) {
		return InitializeBin(bin);
	}
	else {
		return it->second;
	}
}

int PlayerInventoryModel::GetBinValue(int bin) {
	return GetBin(bin).Value;
}

void PlayerInventoryModel::SetBinValue(int bin, int value) {
	const auto& base_bin = BaseInventory.GetBin(bin);
	GetBin(bin).Value = std::min(std::max(value, base_bin.MinValue), base_bin.MaxValue);
}

void PlayerInventoryModel::ModBinValue(int bin, int delta) {
	SetBinValue(bin, GetBin(bin).Value + delta);
}

float PlayerInventoryModel::GetBinCooldown(int bin) {
	return GetBin(bin).Cooldown;
}

void PlayerInventoryModel::SetBinCooldown(int bin, float value) {
	GetBin(bin).Cooldown = value;
}

bool PlayerInventoryModel::IsBinActivated(int bin) {
	return GetBin(bin).Activated;
}

void PlayerInventoryModel::SetBinActivated(int bin, bool state) {
	GetBin(bin).Activated = state;
}

bool PlayerInventoryModel::IsBinAvailable(int bin) {
	return GetBin(bin).Available;
}

void PlayerInventoryModel::SetBinAvailable(int bin, bool state) {
	GetBin(bin).Available = state;
}

void PlayerInventoryModel::ModAllCooldowns(float value) {
	for(auto& bin_pair : bins) {
		std::get<1>(bin_pair).Cooldown = std::max(std::get<1>(bin_pair).Cooldown + value, 0.0f);
	}
}

InventoryModel::InventoryModel(const Content::Assets::Inventory& BaseInventory)
	: BaseInventory(BaseInventory) {
	return;
}

PlayerInventoryModel& InventoryModel::GetInventory(int player_id) {
	auto it = player_inventories.find(player_id);
	if(it == player_inventories.end()) {
		return std::get<0>(player_inventories.emplace(player_id, PlayerInventoryModel(BaseInventory)))->second;
	}
	else {
		return it->second;
	}
}

void InventoryModel::ModAllCooldowns(float dt) {
	for(auto& inv_pair : player_inventories) {
		std::get<1>(inv_pair).ModAllCooldowns(dt);
	}
}
