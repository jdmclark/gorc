#include "inventorypresenter.h"
#include "game/world/level/levelmodel.h"
#include "inventorymodel.h"
#include "cog/verbs/table.h"
#include "game/components.h"
#include "game/world/level/levelpresenter.h"

Gorc::Game::World::Level::Gameplay::InventoryPresenter::InventoryPresenter(LevelPresenter& presenter)
	: presenter(presenter) {
	return;
}

void Gorc::Game::World::Level::Gameplay::InventoryPresenter::Start(LevelModel& levelModel, InventoryModel& model) {
	this->model = &model;
	this->levelModel = &levelModel;
	return;
}

void Gorc::Game::World::Level::Gameplay::InventoryPresenter::Update(double dt) {
	model->ModAllCooldowns(-dt);
}

void Gorc::Game::World::Level::Gameplay::InventoryPresenter::ChangeInv(int player, int bin, int amount) {
	model->GetInventory(player).ModBinValue(bin, amount);
}

int Gorc::Game::World::Level::Gameplay::InventoryPresenter::GetInv(int player, int bin) {
	return model->GetInventory(player).GetBinValue(bin);
}

int Gorc::Game::World::Level::Gameplay::InventoryPresenter::GetInvCog(int player, int bin) {
	const Content::Assets::InventoryBin& inv_bin = model->BaseInventory.GetBin(bin);
	if(inv_bin.Cog) {
		return presenter.ScriptPresenter.GetGlobalCogInstance(&inv_bin.Cog->Script);
	}
	else {
		return -1;
	}
}

int Gorc::Game::World::Level::Gameplay::InventoryPresenter::GetInvMax(int player, int bin) {
	return model->BaseInventory.GetBin(bin).MaxValue;
}

int Gorc::Game::World::Level::Gameplay::InventoryPresenter::GetInvMin(int player, int bin) {
	return model->BaseInventory.GetBin(bin).MinValue;
}

bool Gorc::Game::World::Level::Gameplay::InventoryPresenter::IsInvActivated(int player, int bin) {
	return model->GetInventory(player).IsBinActivated(bin);
}

bool Gorc::Game::World::Level::Gameplay::InventoryPresenter::IsInvAvailable(int player, int bin) {
	return model->GetInventory(player).IsBinAvailable(bin);
}

void Gorc::Game::World::Level::Gameplay::InventoryPresenter::SetBinWait(int player, int bin, float delay) {
	model->GetInventory(player).SetBinCooldown(bin, delay);
}

void Gorc::Game::World::Level::Gameplay::InventoryPresenter::SetInv(int player, int bin, int value) {
	model->GetInventory(player).SetBinValue(bin, value);
}

void Gorc::Game::World::Level::Gameplay::InventoryPresenter::SetInvActivated(int player, int bin, bool value) {
	model->GetInventory(player).SetBinActivated(bin, value);
}

void Gorc::Game::World::Level::Gameplay::InventoryPresenter::SetInvAvailable(int player, int bin, bool value) {
	model->GetInventory(player).SetBinAvailable(bin, value);
}

void Gorc::Game::World::Level::Gameplay::InventoryPresenter::OnItemHotkeyPressed(int player, int bin) {
	// TODO: If weapon, change 'bin' to 0 for primary, 1 for secondary fire.
	presenter.ScriptPresenter.SendMessage(GetInvCog(player, bin), Cog::MessageId::Activated,
			-1, bin, Flags::MessageType::System,
			player, Flags::MessageType::Thing);
}

void Gorc::Game::World::Level::Gameplay::InventoryPresenter::OnItemHotkeyReleased(int player, int bin) {
	// TODO: If weapon, change 'bin' to 0 for primary, 1 for secondary fire.
	presenter.ScriptPresenter.SendMessage(GetInvCog(player, bin), Cog::MessageId::Deactivated,
			-1, bin, Flags::MessageType::System,
			player, Flags::MessageType::Thing);
}

void Gorc::Game::World::Level::Gameplay::InventoryPresenter::RegisterVerbs(Cog::Verbs::VerbTable& verbTable, Components& components) {
	verbTable.AddVerb<void, 3>("changeinv", [&components](int player, int bin, int amount) {
		components.CurrentLevelPresenter->InventoryPresenter.ChangeInv(player, bin, amount);
	});

	verbTable.AddVerb<int, 2>("getinv", [&components](int player, int bin) {
		return components.CurrentLevelPresenter->InventoryPresenter.GetInv(player, bin);
	});

	verbTable.AddVerb<int, 2>("getinvmax", [&components](int player, int bin) {
		return components.CurrentLevelPresenter->InventoryPresenter.GetInvMax(player, bin);
	});

	verbTable.AddVerb<int, 2>("getinvmin", [&components](int player, int bin) {
		return components.CurrentLevelPresenter->InventoryPresenter.GetInvMin(player, bin);
	});

	verbTable.AddVerb<bool, 2>("isinvactivated", [&components](int player, int bin) {
		return components.CurrentLevelPresenter->InventoryPresenter.IsInvActivated(player, bin);
	});

	verbTable.AddVerb<bool, 2>("isinvavailable", [&components](int player, int bin) {
		return components.CurrentLevelPresenter->InventoryPresenter.IsInvAvailable(player, bin);
	});

	verbTable.AddVerb<void, 3>("setbinwait", [&components](int player, int bin, float value) {
		components.CurrentLevelPresenter->InventoryPresenter.SetBinWait(player, bin, value);
	});

	verbTable.AddVerb<void, 3>("setinv", [&components](int player, int bin, int value) {
		components.CurrentLevelPresenter->InventoryPresenter.SetInv(player, bin, value);
	});

	verbTable.AddVerb<void, 3>("setinvactivated", [&components](int player, int bin, bool value) {
		components.CurrentLevelPresenter->InventoryPresenter.SetInvActivated(player, bin, value);
	});

	verbTable.AddVerb<void, 3>("setinvavailable", [&components](int player, int bin, bool value) {
		components.CurrentLevelPresenter->InventoryPresenter.SetInvAvailable(player, bin, value);
	});
}
