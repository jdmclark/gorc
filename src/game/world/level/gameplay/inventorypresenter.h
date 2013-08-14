#pragma once

namespace Gorc {
namespace Cog {
namespace Verbs {
class VerbTable;
}
}

namespace Game {
class Components;

namespace World {
namespace Level {
class LevelModel;
class LevelPresenter;

namespace Gameplay {

class InventoryModel;

class InventoryPresenter {
private:
	LevelPresenter& presenter;
	InventoryModel* model;
	LevelModel* levelModel;

public:
	InventoryPresenter(LevelPresenter& presenter);

	void Start(LevelModel& levelModel, InventoryModel& model);
	void Update(double dt);

	void ChangeInv(int player, int bin, int amount);
	int GetInvCog(int player, int bin);
	int GetInv(int player, int bin);
	int GetInvMax(int player, int bin);
	int GetInvMin(int player, int bin);
	bool IsInvActivated(int player, int bin);
	bool IsInvAvailable(int player, int bin);
	void SetBinWait(int player, int bin, float delay);
	void SetInv(int player, int bin, int value);
	void SetInvActivated(int player, int bin, bool value);
	void SetInvAvailable(int player, int bin, bool value);

	void OnItemHotkeyPressed(int player, int bin);
	void OnItemHotkeyReleased(int player, int bin);

	static void RegisterVerbs(Cog::Verbs::VerbTable& verbTable, Components&);
};

}
}
}
}
}
