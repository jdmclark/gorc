#pragma once

#include "frame.h"
#include "static_text.h"
#include <functional>

namespace gorc {
namespace gui {
class gui_view;

namespace widgets {

class edit_box : public widget {
private:
	const content::assets::texture& skin;
	const content::assets::font& font;
	frame* container_box;
	size_t cursor_position = 5;
	size_t scroll_position = 0;
	std::string buffer;
	bool has_focus = false;
	timestamp focus_time = timestamp(0);
	void set_cursor_position(size_t pos, const time& time);
	void set_scroll_position(size_t pos);

public:
	edit_box(gui_view&, const content::assets::texture& skin, const content::assets::font& font, const std::string& text);

	virtual box<2, int> get_minimum_size(const gui_view&) const override;

	virtual bool wants_mouse_input() const override;
	virtual void on_mouse_down(const time& time, const vector<2, int>& position, sf::Mouse::Button button) override;

	virtual bool wants_keyboard_focus() const override;
	virtual void on_gain_keyboard_focus(const time& time) override;
	virtual void on_lost_keyboard_focus(const time& time) override;
	virtual void on_text_entered(const time& time, char ch) override;
	virtual void on_key_down(const time& time, sf::Keyboard::Key, bool shift_down, bool ctrl_down, bool alt_down) override;

	virtual void draw(const time& time, graphics::gui_renderer&, int depth) const override;

	inline void clear() {
		buffer.clear();
		scroll_position = 0;
		cursor_position = 0;
	}

	inline const std::string& get_text() {
		return buffer;
	}
};

}
}
}
