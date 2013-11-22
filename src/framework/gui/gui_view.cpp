#include "gui_view.h"
#include "framework/utility/make_unique.h"
#include "framework/gui/widgets/panel.h"
#include <type_traits>
#include "framework/utility/pair_range.h"

gorc::gui::gui_view::gui_view(const content::assets::shader& gui_shader)
	: input_adapter(*this), root_widget(*parent_widget_map.emplace(nullptr, make_unique<widgets::panel>(*this))->second),
	  gui_renderer(gui_shader) {
	return;
}

void gorc::gui::gui_view::resize_widget(const box<2, int>& position, widget& w) {
	w.position = position;

	for(const auto& child_pair : get_children(w)) {
		auto child_size = w.get_child_position(*this, *child_pair.second);
		resize_widget(child_size, *child_pair.second);
	}
}

void gorc::gui::gui_view::draw_widget(const time& time, const widget& w, int depth) {
	if(!w.visible) {
		return;
	}

	w.draw(time, gui_renderer, depth);

	for(const auto& child_pair : get_children(w)) {
		draw_widget(time, *child_pair.second, depth + child_pair.second->zbias);
	}
}

void gorc::gui::gui_view::layout() {
	resize_widget(get_root().position, get_root());
}

void gorc::gui::gui_view::resize(const box<2, int>& view_size) {
	resize_widget(view_size, get_root());
}

void gorc::gui::gui_view::draw(const time& time, const box<2, int>& view_size, graphics::render_target&) {
	gui_renderer.begin(view_size);
	draw_widget(time, get_root(), get_root().zbias);
	gui_renderer.end();
}

gorc::maybe<gorc::input::input_adapter*> gorc::gui::gui_view::get_input_adapter() {
	return &input_adapter;
}

std::tuple<int, gorc::maybe<gorc::gui::widget*>> gorc::gui::gui_view::get_mouse_over_widget(const vector<2, int>& cursor_pos, widget& w, int depth) {
	if(!w.visible) {
		return std::make_tuple(std::numeric_limits<int>::lowest(), maybe<widget*>());
	}

	if(w.wants_mouse_input() && w.position.contains(cursor_pos)) {
		return std::make_tuple(depth, &w);
	}

	auto rv = std::make_tuple(std::numeric_limits<int>::lowest(), maybe<widget*>());
	for(auto& child_pair : get_children(w)) {
		auto mow = get_mouse_over_widget(cursor_pos, *child_pair.second, depth + child_pair.second->zbias);
		if(std::get<1>(mow) && std::get<0>(mow) >= std::get<0>(rv)) {
			rv = mow;
		}
	}

	return rv;
}

void gorc::gui::gui_view::set_mouse_cursor_position(const time& time, const vector<2, int>& cursor_pos) {
	maybe<widget*> new_mouse_over_widget = std::get<1>(get_mouse_over_widget(cursor_pos, get_root(), get_root().zbias));
	if(mouse_over_widget != new_mouse_over_widget) {
		if_set(mouse_over_widget, then_do, [&time](widget& w) {
			w.on_mouse_out(time);
		});

		if_set(new_mouse_over_widget, then_do, [&time](widget& w) {
			w.on_mouse_over(time);
		});

		mouse_over_widget = new_mouse_over_widget;
	}
}

bool gorc::gui::gui_view::wants_mouse_focus() const {
	return mouse_over_widget || keyboard_focus_widget;
}

bool gorc::gui::gui_view::wants_keyboard_focus() const {
	return keyboard_focus_widget;
}

void gorc::gui::gui_view::on_mouse_button_up(const time& time, const vector<2, int>& position, sf::Mouse::Button b) {
	if_set(mouse_over_widget, then_do, [&](widget& w) {
		auto top_left = make_vector(std::get<0>(get_range<0>(w.position)), std::get<0>(get_range<1>(w.position)));
		w.on_mouse_up(time, position - top_left, b);
	});
}

void gorc::gui::gui_view::on_mouse_button_down(const time& time, const vector<2, int>& position, sf::Mouse::Button b) {
	if_set(mouse_over_widget, then_do, [&](widget& w) {
		auto top_left = make_vector(std::get<0>(get_range<0>(w.position)), std::get<0>(get_range<1>(w.position)));
		w.on_mouse_down(time, position - top_left, b);
	});

	if(mouse_over_widget != keyboard_focus_widget) {
		if_set(keyboard_focus_widget, then_do, [&time](widget& old_kw) {
			old_kw.on_lost_keyboard_focus(time);
		});


		if_set(mouse_over_widget, then_do, [&time](widget& new_kw) {
			new_kw.on_gain_keyboard_focus(time);
		});

		keyboard_focus_widget = mouse_over_widget;
	}
}

void gorc::gui::gui_view::on_keyboard_key_up(const time& time, sf::Keyboard::Key k, bool shift_down, bool ctrl_down, bool alt_down) {
	if_set(keyboard_focus_widget, then_do, [&](widget& w) {
		w.on_key_up(time, k, shift_down, ctrl_down, alt_down);
	});
}

void gorc::gui::gui_view::on_keyboard_key_down(const time& time, sf::Keyboard::Key k, bool shift_down, bool ctrl_down, bool alt_down) {
	if_set(keyboard_focus_widget, then_do, [&](widget& w) {
		w.on_key_down(time, k, shift_down, ctrl_down, alt_down);
	});
}

void gorc::gui::gui_view::on_keyboard_text_entered(const time& time, char ch) {
	if_set(keyboard_focus_widget, then_do, [&time, ch](widget& w) {
		w.on_text_entered(time, ch);
	});
}
