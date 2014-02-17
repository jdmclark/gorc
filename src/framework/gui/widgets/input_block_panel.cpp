#include "input_block_panel.h"

gorc::gui::widgets::input_block_panel::input_block_panel(gui_view&, std::function<void()>&& click_predicate)
    : click_predicate(click_predicate) {
    dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };
    return;
}

bool gorc::gui::widgets::input_block_panel::wants_mouse_input() const {
    return true;
}

void gorc::gui::widgets::input_block_panel::on_mouse_down(const time& time, const vector<2, int>& position, sf::Mouse::Button button) {
    click_predicate();
}
