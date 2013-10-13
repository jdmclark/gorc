#pragma once

#include <unordered_map>
#include "content/assets/inventory.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Gameplay {

class PlayerBinModel {
public:
	bool Activated = false;
	bool Available = false;
	int Value = 0;
	float Cooldown = 0.0f;
};

class PlayerInventoryModel {
private:
	const Content::Assets::Inventory& BaseInventory;
	std::unordered_map<int, PlayerBinModel> bins;

	PlayerBinModel& InitializeBin(int bin);
	PlayerBinModel& GetBin(int bin);

public:
	PlayerInventoryModel(const Content::Assets::Inventory& BaseInventory);

	int GetBinValue(int bin);
	void SetBinValue(int bin, int value);
	void ModBinValue(int bin, int delta);

	float GetBinCooldown(int bin);
	void SetBinCooldown(int bin, float value);

	bool IsBinActivated(int bin);
	void SetBinActivated(int bin, bool value);

	bool IsBinAvailable(int bin);
	void SetBinAvailable(int bin, bool value);

	void ModAllCooldowns(float dt);
};

class InventoryModel {
private:
	std::unordered_map<int, PlayerInventoryModel> player_inventories;

public:
	const Content::Assets::Inventory& BaseInventory;

	InventoryModel(const Content::Assets::Inventory& BaseInventory);

	PlayerInventoryModel& GetInventory(int player_id);
	void ModAllCooldowns(float dt);
};

}
}
}
}
}
