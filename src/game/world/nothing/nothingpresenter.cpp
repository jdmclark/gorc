#include "nothingpresenter.h"

Gorc::Game::World::Nothing::NothingPresenter::NothingPresenter(Components& components)
	: components(components) {
	return;
}

void Gorc::Game::World::Nothing::NothingPresenter::Start(Event::EventBus& eventBus) {
	components.WorldViewFrame.SetView(components.NothingView);
}
