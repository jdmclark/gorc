#include "actionpresenter.h"
#include "framework/events/exitevent.h"
#include "game/events/windowfocusevent.h"

using namespace Gorc::Math;

Gorc::Game::Screen::Action::ActionPresenter::ActionPresenter(Components& components, const ActionPlace& place)
	: components(components), place(place) {
	return;
}

void Gorc::Game::Screen::Action::ActionPresenter::Start(Event::EventBus& eventBus) {
	eventBus.AddHandler<Game::Events::WindowFocusEvent>([this](Game::Events::WindowFocusEvent& evt) {
		window_has_focus = evt.HasFocus;
	});

	components.ActionView.SetPresenter(this);
	components.ScreenViewFrame.SetView(components.ActionView);

	components.Window.ShowMouseCursor(false);
	components.Window.SetCursorPosition(components.Window.GetWidth() / 2, components.Window.GetHeight() / 2);
	return;
}

void Gorc::Game::Screen::Action::ActionPresenter::Update(double dt) {
	// General controls
	if(components.Input.IsKeyDown(sf::Key::Escape)) {
		Gorc::Events::ExitEvent evt;
		components.EventBus.FireEvent(evt);
	}

	if(r_key_down && !components.Input.IsKeyDown(sf::Key::R)) {
		r_key_down = false;
	}
	else if(!r_key_down && components.Input.IsKeyDown(sf::Key::R)) {
		r_key_down = true;
		components.CurrentLevelPresenter->Respawn();
	}

	if(space_key_down && !components.Input.IsKeyDown(sf::Key::Space)) {
		space_key_down = false;
	}
	else if(!space_key_down && components.Input.IsKeyDown(sf::Key::Space)) {
		space_key_down = true;
		components.CurrentLevelPresenter->Jump();
	}

	if(z_key_down && !components.Input.IsMouseButtonDown(sf::Mouse::Left)) {
		z_key_down = false;
	}
	else if(!z_key_down && components.Input.IsMouseButtonDown(sf::Mouse::Left)) {
		z_key_down = true;
		components.CurrentLevelPresenter->Damage();
	}

	if(x_key_down && !components.Input.IsKeyDown(sf::Key::E)) {
		x_key_down = false;
	}
	else if(!x_key_down && components.Input.IsKeyDown(sf::Key::E)) {
		x_key_down = true;
		components.CurrentLevelPresenter->Activate();
	}

	if(f1_key_down && !components.Input.IsKeyDown(sf::Key::F1)) {
		f1_key_down = false;
	}
	else if(!f1_key_down && components.Input.IsKeyDown(sf::Key::F1)) {
		f1_key_down = true;
		components.CurrentLevelPresenter->ToggleFieldLight();
	}

	// Camera rotate
	if(window_has_focus) {
		Vector<2, double> ScreenCenter = Vec(static_cast<double>(components.Window.GetWidth()) / 2.0, static_cast<double>(components.Window.GetHeight()) / 2.0);
		Vector<2, double> CursorPos = (Vec(static_cast<double>(components.Input.GetMouseX()),
				static_cast<double>(components.Input.GetMouseY())) - ScreenCenter) / Get<X>(ScreenCenter);
		components.Window.SetCursorPosition(components.Window.GetWidth() / 2, components.Window.GetHeight() / 2);

		auto CameraRotation = -CursorPos * 180.0 * dt;

		components.CurrentLevelPresenter->YawCamera(Get<X>(CameraRotation));
		components.CurrentLevelPresenter->PitchCamera(Get<Y>(CameraRotation));
	}

	// Camera translate
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

	if(Length2(Translate) > std::numeric_limits<float>::epsilon()) {
		Translate = Normalize(Translate);
	}
	else {
		Translate = Zero<3>();
	}

	components.CurrentLevelPresenter->TranslateCamera(Translate);
}
