#include "corpse_controller.h"
#include "game/world/level/level_presenter.h"
#include "game/world/level/level_model.h"

void Gorc::Game::World::Level::Gameplay::CorpseController::CreateControllerData(int thing_id) {
	return;
}

void Gorc::Game::World::Level::Gameplay::CorpseController::RemoveControllerData(int thing_id) {
	return;
}

void Gorc::Game::World::Level::Gameplay::CorpseController::HandleAnimationMarker(int thing_id, enum Flags::KeyMarkerType marker) {
	if(marker == Flags::KeyMarkerType::Death) {
		// Play death thud
		presenter.SoundPresenter.PlaySoundClass(thing_id, Flags::SoundSubclassType::CorpseHit);
	}
}
