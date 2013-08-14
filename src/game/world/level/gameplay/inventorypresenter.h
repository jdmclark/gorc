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

namespace Gameplay {

class InventoryModel;

class InventoryPresenter {
private:
	InventoryModel* model;
	LevelModel* levelModel;

public:
	void Start(LevelModel& levelModel, InventoryModel& model);
	void Update(double dt);

	void ChangeInv(int player, int bin, int amount);
	int GetInv(int player, int bin);
	int GetInvMax(int player, int bin);
	int GetInvMin(int player, int bin);
	bool IsInvActivated(int player, int bin);
	bool IsInvAvailable(int player, int bin);
	void SetBinWait(int player, int bin, float delay);
	void SetInv(int player, int bin, int value);
	void SetInvActivated(int player, int bin, bool value);
	void SetInvAvailable(int player, int bin, bool value);

	static void RegisterVerbs(Cog::Verbs::VerbTable& verbTable, Components&);
};

}
}
}
}
}
