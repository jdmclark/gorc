#include "action_presenter.h"
#include "game/world/level/level_model.h"
#include "framework/events/exit_event.h"
#include "game/events/window_focus_event.h"

using namespace gorc::math;

gorc::game::screen::action::action_presenter::action_presenter(class components& components, const action_place& place)
	: components(components), place(place) {
	return;
}

void gorc::game::screen::action::action_presenter::start(event::event_bus& eventBus) {
	eventBus.add_handler<game::events::window_focus_event>([this](game::events::window_focus_event& evt) {
		window_has_focus = evt.has_focus;
	});

	components.action_view.set_presenter(this);
	components.screen_view_frame.set_view(components.action_view);

	components.window.setMouseCursorVisible(false);
	sf::Mouse::setPosition(sf::Vector2i(components.window.getSize().x / 2, components.window.getSize().y / 2), components.window);
	return;
}

void gorc::game::screen::action::action_presenter::update(double dt) {
	// General controls
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
		gorc::events::exit_event evt;
		components.event_bus.fire_event(evt);
	}

	if(r_key_down && !sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
		r_key_down = false;
	}
	else if(!r_key_down && sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
		r_key_down = true;
		components.current_level_presenter->respawn();
	}

	if(space_key_down && !sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		space_key_down = false;
	}
	else if(!space_key_down && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		space_key_down = true;
		components.current_level_presenter->jump();
	}

	if(z_key_down && !sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		z_key_down = false;
	}
	else if(!z_key_down && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		z_key_down = true;
		components.current_level_presenter->damage();
	}

	if(x_key_down && !sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
		x_key_down = false;
	}
	else if(!x_key_down && sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
		x_key_down = true;
		components.current_level_presenter->activate();
	}

	int player = components.current_level_presenter->get_local_player_thing();
	if(f1_key_down && !sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) {
		f1_key_down = false;
		components.current_level_presenter->inventory_presenter.on_item_hotkey_released(player, 42);
	}
	else if(!f1_key_down && sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) {
		f1_key_down = true;
		components.current_level_presenter->inventory_presenter.on_item_hotkey_pressed(player, 42);
	}

	if(g_key_down && !sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
		g_key_down = false;
	}
	else if(!g_key_down && sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
		g_key_down = true;
		// Give batteries
		components.current_level_presenter->inventory_presenter.set_inv(player, 13, 200);
	}

	// Camera rotate
	if(window_has_focus) {
		vector<2, double> ScreenCenter = make_vector(static_cast<double>(components.window.getSize().x) / 2.0,
				static_cast<double>(components.window.getSize().y) / 2.0);
		vector<2, double> CursorPos = (make_vector(static_cast<double>(sf::Mouse::getPosition(components.window).x),
				static_cast<double>(sf::Mouse::getPosition(components.window).y)) - ScreenCenter) / get<0>(ScreenCenter);
		sf::Mouse::setPosition(sf::Vector2i(components.window.getSize().x / 2, components.window.getSize().y / 2), components.window);

		auto CameraRotation = -CursorPos * 180.0 * dt;

		components.current_level_presenter->yaw_camera(get<0>(CameraRotation));
		components.current_level_presenter->pitch_camera(get<1>(CameraRotation));
	}

	// Camera translate
	vector<3> Translate = make_zero_vector<3, float>();
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		Translate += make_vector(0.0f, 1.0f, 0.0f);
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		Translate += make_vector(-1.0f, 0.0f, 0.0f);
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		Translate += make_vector(0.0f, -1.0f, 0.0f);
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		Translate += make_vector(1.0f, 0.0f, 0.0f);
	}

	if(length_squared(Translate) > std::numeric_limits<float>::epsilon()) {
		Translate = normalize(Translate);
	}
	else {
		Translate = make_zero_vector<3, float>();
	}

	components.current_level_presenter->translate_camera(Translate);
}
