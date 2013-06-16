#include "actionpresenter.h"
#include "framework/events/exitevent.h"

using namespace Gorc::Math;

Gorc::Game::Screen::Action::ActionPresenter::ActionPresenter(Components& components, const ActionPlace& place)
	: components(components), place(place) {
	return;
}

void Gorc::Game::Screen::Action::ActionPresenter::Start(Event::EventBus& eventBus) {
	components.ActionView.SetPresenter(this);
	components.ScreenViewFrame.SetView(components.ActionView);
	return;
}

void Gorc::Game::Screen::Action::ActionPresenter::Update(double dt) {
	if(components.Input.IsKeyDown(sf::Key::Escape)) {
		Gorc::Events::ExitEvent evt;
		components.EventBus.FireEvent(evt);
	}

	Vector<3> Translate = Zero<3>();
	if(components.Input.IsKeyDown(sf::Key::W)) {
		Translate += Vec(0.0f, 1.0f, 0.0f);
	}

	if(components.Input.IsKeyDown(sf::Key::A)) {
		Translate += Vec(-1.0f, 0.0f, 0.0f);
	}

	if(components.Input.IsKeyDown(sf::Key::S)) {
		Translate += Vec(0.0f, -1.0f, 0.0f);
	}

	if(components.Input.IsKeyDown(sf::Key::D)) {
		Translate += Vec(1.0f, 0.0f, 0.0f);
	}

	if(components.Input.IsKeyDown(sf::Key::Q)) {
		components.LevelView.YawCamera(dt * 3.0f);
	}

	if(components.Input.IsKeyDown(sf::Key::E)) {
		components.LevelView.YawCamera(-dt * 3.0f);
	}

	if(components.Input.IsKeyDown(sf::Key::PageUp)) {
		components.LevelView.PitchCamera(-dt * 1.5f);
	}

	if(components.Input.IsKeyDown(sf::Key::PageDown)) {
		components.LevelView.PitchCamera(dt * 1.5f);
	}

	if(Length2(Translate) > std::numeric_limits<float>::epsilon()) {
		Translate = Normalize(Translate);
	}
	else {
		Translate = Zero<3>();
	}

	components.LevelView.TranslateCamera(Translate);
}
