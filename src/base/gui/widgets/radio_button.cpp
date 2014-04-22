#include "radio_button.h"
#include "base/gui/widgets/panel.h"
#include "base/gui/gui_view.h"

gorc::gui::widgets::radio_button::radio_button(gui_view& view, const content::assets::texture& skin,
        const content::assets::font& font, const std::string& text) : skin(skin), parent_view(view) {
    auto& container = view.add_child<panel>(*this);
    container.padding = 0;
    container.padding.left = 15;

    auto& label = view.add_child<static_text>(container, font, text);
    label.horizontal_align = layout::horizontal_align_style::left;
    label.vertical_align = layout::vertical_align_style::middle;
}

void gorc::gui::widgets::radio_button::select_radio_button(widget& w) {
    radio_button* b = dynamic_cast<radio_button*>(&w);
    if(b) {
        b->selected = b == this;
    }

    for(auto& child_pair : parent_view.get_children(w)) {
        select_radio_button(*child_pair.second);
    }
}

bool gorc::gui::widgets::radio_button::wants_mouse_input() const {
    return true;
}

void gorc::gui::widgets::radio_button::on_mouse_out(const time&) {
    is_depressed = false;
}

void gorc::gui::widgets::radio_button::on_mouse_down(const time&, const vector<2, int>&, sf::Mouse::Button btn) {
    if(btn == sf::Mouse::Left) {
        is_depressed = true;
    }
}

void gorc::gui::widgets::radio_button::on_mouse_up(const time&, const vector<2, int>&, sf::Mouse::Button btn) {
    if(btn == sf::Mouse::Left && is_depressed) {
        is_depressed = false;
        select_radio_button(parent_view.get_root());
    }
}

void gorc::gui::widgets::radio_button::draw(const time&, graphics::gui_renderer& r, int depth) const {
    box<2, int> tex_coords = (selected) ?
            make_box(make_vector(21, 39), make_vector(34, 52)) :
            make_box(make_vector(21, 26), make_vector(34, 39));
    int top, left;
    std::tie(left, std::ignore) = get_range<0>(position);
    std::tie(top, std::ignore) = get_range<1>(position);
    r.draw_sprite(skin, make_box(make_vector(left, top), make_vector(left + 13, top + 13)), tex_coords, depth);
}
