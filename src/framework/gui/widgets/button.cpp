#include "button.h"
#include "framework/gui/gui_view.h"

gorc::gui::widgets::button::button(gui_view& view, const content::assets::texture& skin,
		const content::assets::font& font, const std::string& text,
		const std::function<void()>& click_predicate) : click_predicate(click_predicate) {
	container_box = &view.add_child<frame>(*this, skin);
	container_box->dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };
	container_box->padding = 2;
	container_box->style = widgets::frame_style::outset;

	label = &view.add_child<static_text>(*container_box, font, text);
	label->horizontal_align = layout::horizontal_align_style::center;
	label->vertical_align = layout::vertical_align_style::middle;
}

bool gorc::gui::widgets::button::wants_mouse_input() const {
	return true;
}

void gorc::gui::widgets::button::on_mouse_out(const time& time) {
	set_is_depressed(false);
}

void gorc::gui::widgets::button::on_mouse_down(const time& time, const vector<2, int>& position, sf::Mouse::Button btn) {
	if(btn == sf::Mouse::Left) {
		set_is_depressed(true);
	}
}

void gorc::gui::widgets::button::on_mouse_up(const time& time, const vector<2, int>& position, sf::Mouse::Button btn) {
	if(btn == sf::Mouse::Left && is_depressed) {
		set_is_depressed(false);
		click_predicate();
	}
}
