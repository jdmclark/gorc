#include "actionpresenter.h"
#include "game/world/level/levelmodel.h"
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

	components.Window.setMouseCursorVisible(false);
	sf::Mouse::setPosition(sf::Vector2i(components.Window.getSize().x / 2, components.Window.getSize().y / 2), components.Window);
	return;
}

void Gorc::Game::Screen::Action::ActionPresenter::Update(double dt) {
	// General controls
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
		Gorc::Events::ExitEvent evt;
		components.EventBus.FireEvent(evt);
	}

	if(r_key_down && !sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
		r_key_down = false;
	}
	else if(!r_key_down && sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
		r_key_down = true;
		components.CurrentLevelPresenter->Respawn();
	}

	if(space_key_down && !sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		space_key_down = false;
	}
	else if(!space_key_down && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		space_key_down = true;
		components.CurrentLevelPresenter->Jump();
	}

	if(z_key_down && !sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		z_key_down = false;
	}
	else if(!z_key_down && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		z_key_down = true;
		components.CurrentLevelPresenter->Damage();
	}

	if(x_key_down && !sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
		x_key_down = false;
	}
	else if(!x_key_down && sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
		x_key_down = true;
		components.CurrentLevelPresenter->Activate();
	}

	int player = components.CurrentLevelPresenter->GetLocalPlayerThing();
	if(f1_key_down && !sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) {
		f1_key_down = false;
		components.CurrentLevelPresenter->InventoryPresenter.OnItemHotkeyReleased(player, 42);
	}
	else if(!f1_key_down && sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) {
		f1_key_down = true;
		components.CurrentLevelPresenter->InventoryPresenter.OnItemHotkeyPressed(player, 42);
	}

	if(g_key_down && !sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
		g_key_down = false;
	}
	else if(!g_key_down && sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
		g_key_down = true;
		// Give batteries
		components.CurrentLevelPresenter->InventoryPresenter.SetInv(player, 13, 200);
	}

	// Camera rotate
	if(window_has_focus) {
		Vector<2, double> ScreenCenter = Vec(static_cast<double>(components.Window.getSize().x) / 2.0,
				static_cast<double>(components.Window.getSize().y) / 2.0);
		Vector<2, double> CursorPos = (Vec(static_cast<double>(sf::Mouse::getPosition(components.Window).x),
				static_cast<double>(sf::Mouse::getPosition(components.Window).y)) - ScreenCenter) / Get<X>(ScreenCenter);
		sf::Mouse::setPosition(sf::Vector2i(components.Window.getSize().x / 2, components.Window.getSize().y / 2), components.Window);

		auto CameraRotation = -CursorPos * 180.0 * dt;

		components.CurrentLevelPresenter->YawCamera(Get<X>(CameraRotation));
		components.CurrentLevelPresenter->PitchCamera(Get<Y>(CameraRotation));
	}

	// Camera translate
	Vector<3> Translate = Zero<3>();
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		Translate += Vec(0.0f, 1.0f, 0.0f);
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		Translate += Vec(-1.0f, 0.0f, 0.0f);
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		Translate += Vec(0.0f, -1.0f, 0.0f);
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
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
