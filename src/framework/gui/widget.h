#pragma once

#include "framework/utility/time.h"
#include "framework/math/box.h"
#include "layout/margin.h"
#include "layout/padding.h"
#include "layout/horizontal_align_style.h"
#include "layout/vertical_align_style.h"
#include "framework/utility/flag_set.h"
#include "layout/dock_style.h"
#include "framework/graphics/gui_renderer.h"

namespace gorc {
namespace gui {

class gui_view;

class widget {
protected:
	widget();
	box<2, int> get_default_child_position(const gui_view&, const widget& child, const box<2, int>& parent_client_area) const;

public:
	bool visible = true;
	int zbias = 1;
	box<2, int> position;
	layout::margin margin;
	layout::padding padding;
	layout::horizontal_align_style horizontal_align = layout::horizontal_align_style::left;
	layout::vertical_align_style vertical_align = layout::vertical_align_style::top;
	flag_set<layout::dock_style> dock_style;

	vector<2, int> grid_position = make_vector(0, 0);
	int column_span = 1;
	int row_span = 1;

	virtual ~widget();

	virtual box<2, int> get_child_position(const gui_view&, const widget& child) const;
	virtual box<2, int> get_minimum_size(const gui_view&) const;

	virtual bool wants_mouse_input() const;
	virtual void on_mouse_over(const time& time);
	virtual void on_mouse_out(const time& time);
	virtual void on_mouse_down(const time& time, const vector<2, int>&, sf::Mouse::Button);
	virtual void on_mouse_up(const time& time, const vector<2, int>&, sf::Mouse::Button);
	virtual bool wants_keyboard_focus() const;
	virtual void on_gain_keyboard_focus(const time& time);
	virtual void on_lost_keyboard_focus(const time& time);
	virtual void on_text_entered(const time& time, char ch);
	virtual void on_key_down(const time& time, sf::Keyboard::Key, bool shift_down, bool ctrl_down, bool alt_down);
	virtual void on_key_up(const time& time, sf::Keyboard::Key, bool shift_down, bool ctrl_down, bool alt_down);

	virtual void draw(const time& time, graphics::gui_renderer&, int depth) const;
};

}
}
