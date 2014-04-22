#include "check_box.h"
#include "base/gui/widgets/panel.h"
#include "base/gui/gui_view.h"

gorc::gui::widgets::check_box::check_box(gui_view& view, const content::assets::texture& skin,
        const content::assets::font& font, const std::string& text) : skin(skin) {
    auto& container = view.add_child<panel>(*this);
    container.padding = 0;
    container.padding.left = 15;

    auto& label = view.add_child<static_text>(container, font, text);
    label.horizontal_align = layout::horizontal_align_style::left;
    label.vertical_align = layout::vertical_align_style::middle;
}

bool gorc::gui::widgets::check_box::wants_mouse_input() const {
    return true;
}

void gorc::gui::widgets::check_box::on_mouse_out(const time&) {
    is_depressed = false;
}

void gorc::gui::widgets::check_box::on_mouse_down(const time&, const vector<2, int>&, sf::Mouse::Button btn) {
    if(btn == sf::Mouse::Left) {
        is_depressed = true;
    }
}

void gorc::gui::widgets::check_box::on_mouse_up(const time&, const vector<2, int>&, sf::Mouse::Button btn) {
    if(btn == sf::Mouse::Left && is_depressed) {
        is_depressed = false;
        selected = !selected;
    }
}

void gorc::gui::widgets::check_box::draw(const time&, graphics::gui_renderer& r, int depth) const {
    box<2, int> tex_coords = (selected) ?
            make_box(make_vector(21, 13), make_vector(34, 26)) :
            make_box(make_vector(21, 0), make_vector(34, 13));
    int top, left;
    std::tie(left, std::ignore) = get_range<0>(position);
    std::tie(top, std::ignore) = get_range<1>(position);
    r.draw_sprite(skin, make_box(make_vector(left, top), make_vector(left + 13, top + 13)), tex_coords, depth);
}
