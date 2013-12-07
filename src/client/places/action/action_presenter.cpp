#include "action_presenter.h"
#include "client/application.h"
#include "game/world/level_presenter.h"

gorc::client::action::action_presenter::action_presenter(application& app)
	: app(app), input_adapter(*this), key_dispatcher(*this), click_dispatcher(*this) {
	return;
}

void gorc::client::action::action_presenter::start(event::event_bus& bus) {
	app.action_view->set_presenter(this);
	app.views.set_layer(view_layer::screen, *app.action_view);

	bus.add_handler<events::window_focus>([this](events::window_focus& evt) {
		window_has_focus = evt.has_focus;
	});

	// Center mouse cursor
	sf::Mouse::setPosition(sf::Vector2i(app.window.getSize().x / 2, app.window.getSize().y / 2), app.window);

	return;
}

void gorc::client::action::action_presenter::handle_mouse_input(const time& time) {
	if(window_has_focus) {
		vector<2, double> ScreenCenter = make_vector(static_cast<double>(app.window.getSize().x) / 2.0,
				static_cast<double>(app.window.getSize().y) / 2.0);
		vector<2, double> CursorPos = (make_vector(static_cast<double>(sf::Mouse::getPosition(app.window).x),
				static_cast<double>(sf::Mouse::getPosition(app.window).y)) - ScreenCenter) / get<0>(ScreenCenter);
		sf::Mouse::setPosition(sf::Vector2i(app.window.getSize().x / 2, app.window.getSize().y / 2), app.window);

		auto CameraRotation = -CursorPos * 20000.0 * time;

		app.components.current_level_presenter->yaw_camera(get<0>(CameraRotation));
		app.components.current_level_presenter->pitch_camera(get<1>(CameraRotation));
	}

	click_dispatcher.handle_mouse_input(time);
}

void gorc::client::action::action_presenter::handle_keyboard_input(const time& time) {
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

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
		app.components.current_level_presenter->fly();
	}

	app.components.current_level_presenter->translate_camera(Translate);

	key_dispatcher.handle_keyboard_input(time);
}

void gorc::client::action::action_presenter::on_mouse_button_down(const time& time, const vector<2, int>& cursor_pos, sf::Mouse::Button b) {
	int current_player = app.components.current_level_presenter->get_local_player_thing();

	if(b == sf::Mouse::Left) {
		app.components.current_level_presenter->inventory_presenter.on_weapon_fire_pressed(current_player, 0);
	}
	else if(b == sf::Mouse::Right) {
		app.components.current_level_presenter->inventory_presenter.on_weapon_fire_pressed(current_player, 1);
	}
}

void gorc::client::action::action_presenter::on_mouse_button_up(const time& time, const vector<2, int>& cursor_pos, sf::Mouse::Button b) {
	int current_player = app.components.current_level_presenter->get_local_player_thing();

	if(b == sf::Mouse::Left) {
		app.components.current_level_presenter->inventory_presenter.on_weapon_fire_released(current_player, 0);
	}
	else if(b == sf::Mouse::Right) {
		app.components.current_level_presenter->inventory_presenter.on_weapon_fire_released(current_player, 1);
	}
}

void gorc::client::action::action_presenter::on_keyboard_key_down(const time& time, sf::Keyboard::Key k, bool shift, bool ctrl, bool alt) {
	int current_player = app.components.current_level_presenter->get_local_player_thing();

	switch(k) {
	case sf::Keyboard::Escape:
		app.event_bus.fire_event(events::exit());
		break;

	case sf::Keyboard::R:
		app.components.current_level_presenter->respawn();
		break;

	case sf::Keyboard::E:
		app.components.current_level_presenter->activate();
		break;

	case sf::Keyboard::Z:
		app.components.current_level_presenter->damage();
		break;

	case sf::Keyboard::F1:
		app.components.current_level_presenter->camera_presenter.cycle_camera();
		break;

	case sf::Keyboard::F2:
		app.components.current_level_presenter->inventory_presenter.on_item_hotkey_pressed(current_player, 42);
		break;

	case sf::Keyboard::G:
		// Give fieldlight batteries, bryar, strifle, xbow, repeater, and ammo.
		app.components.current_level_presenter->inventory_presenter.set_inv(current_player, 13, 200);
		app.components.current_level_presenter->inventory_presenter.set_inv(current_player, 2, 1);
		app.components.current_level_presenter->inventory_presenter.set_inv(current_player, 3, 1);
		app.components.current_level_presenter->inventory_presenter.set_inv(current_player, 5, 1);
		app.components.current_level_presenter->inventory_presenter.set_inv(current_player, 6, 1);
		app.components.current_level_presenter->inventory_presenter.set_inv(current_player, 11, 500);
		app.components.current_level_presenter->inventory_presenter.set_inv(current_player, 12, 500);
		break;

	case sf::Keyboard::Space:
		app.components.current_level_presenter->jump();
		break;

	case sf::Keyboard::LControl:
		app.components.current_level_presenter->crouch(true);
		break;

	case sf::Keyboard::Num1:
		app.components.current_level_presenter->inventory_presenter.select_weapon(current_player, 1);
		break;

	case sf::Keyboard::Num2:
		app.components.current_level_presenter->inventory_presenter.select_weapon(current_player, 2);
		break;

	case sf::Keyboard::Num3:
		app.components.current_level_presenter->inventory_presenter.select_weapon(current_player, 3);
		break;

	case sf::Keyboard::Num5:
		app.components.current_level_presenter->inventory_presenter.select_weapon(current_player, 5);
		break;

	case sf::Keyboard::Num6:
		app.components.current_level_presenter->inventory_presenter.select_weapon(current_player, 6);
		break;

	default:
		break;
	}
}

void gorc::client::action::action_presenter::on_keyboard_key_up(const time& time, sf::Keyboard::Key k, bool shift, bool ctrl, bool alt) {
	switch(k) {
	case sf::Keyboard::F2:
		app.components.current_level_presenter->inventory_presenter.on_item_hotkey_released(app.components.current_level_presenter->get_local_player_thing(), 42);
		break;

	case sf::Keyboard::LControl:
		app.components.current_level_presenter->crouch(false);
		break;

	default:
		break;
	}
}
