#include "widget.h"
#include "gui_view.h"

gorc::gui::widget::widget() : position(make_box(make_vector(0, 0), make_vector(0, 0))) {
    return;
}

gorc::gui::widget::~widget() {
    return;
}

gorc::box<2, int> gorc::gui::widget::get_default_child_position(const gui_view& view, const widget& child,
        const box<2, int>& parent_area) const {
    auto min_size = child.get_minimum_size(view);

    // Compute width
    int desired_width = get_size<0>(min_size);
    if(child.dock_style & layout::dock_style::width) {
        // Widget must be the size of the owning panel.
        desired_width = std::max(get_size<0>(parent_area), desired_width);
    }

    // Compute height
    int desired_height = get_size<1>(min_size);
    if(child.dock_style & layout::dock_style::height) {
        // Widget must be the size of the owning panel.
        desired_height = std::max(get_size<1>(parent_area), desired_height);
    }

    int desired_x = get<0>(child.position.v0);
    switch(child.horizontal_align) {
    case layout::horizontal_align_style::left:
        desired_x = std::get<0>(get_range<0>(parent_area)) + child.padding.left;
        break;

    case layout::horizontal_align_style::right:
        desired_x = std::get<1>(get_range<0>(parent_area)) - desired_width - child.padding.right;
        break;

    case layout::horizontal_align_style::center:
        desired_x = std::get<0>(get_range<0>(parent_area)) + (get_size<0>(parent_area) - desired_width) / 2  + child.padding.left - child.padding.right;
        break;
    }

    int desired_y = get<1>(child.position.v0);
    switch(child.vertical_align) {
    case layout::vertical_align_style::top:
        desired_y = std::get<0>(get_range<1>(parent_area)) + child.padding.top;
        break;

    case layout::vertical_align_style::bottom:
        desired_y = std::get<1>(get_range<1>(parent_area)) - desired_height - child.padding.bottom;
        break;

    case layout::vertical_align_style::middle:
        desired_y = std::get<0>(get_range<1>(parent_area)) + (get_size<1>(parent_area) - desired_height) / 2 + child.padding.top - child.padding.bottom;
        break;
    }

    auto top_left = make_vector(desired_x, desired_y) + make_vector(child.margin.left, child.margin.top);
    auto bottom_right = make_vector(desired_x, desired_y) + make_vector(desired_width, desired_height) - make_vector(child.margin.right, child.margin.bottom);

    return make_box(top_left, bottom_right);
}

gorc::box<2, int> gorc::gui::widget::get_minimum_size(const gui_view& view) const {
    int desired_width = 0;
    int desired_height = 0;

    for(const auto& child : view.get_children(*this)) {
        auto n_smallest = child.second->get_minimum_size(view);
        desired_width = std::max(desired_width, get_size<0>(n_smallest));
        desired_height = std::max(desired_height, get_size<1>(n_smallest));
    }

    return make_box(make_vector(0, 0),
            make_vector(desired_width + padding.left + padding.right + margin.left + margin.right,
            desired_height + padding.top + padding.bottom + margin.top + margin.bottom));
}

gorc::box<2, int> gorc::gui::widget::get_child_position(const gui_view& v, const widget& child) const {
    int left, right, top, bottom;
    std::tie(left, right) = get_range<0>(position);
    std::tie(top, bottom) = get_range<1>(position);
    auto parent_area = make_box(make_vector(left + padding.left, top + padding.top),
            make_vector(right - padding.right, bottom - padding.bottom));
    return get_default_child_position(v, child, parent_area);
}

bool gorc::gui::widget::wants_mouse_input() const {
    return false;
}

void gorc::gui::widget::on_mouse_over(const time&) {
    return;
}

void gorc::gui::widget::on_mouse_out(const time&) {
    return;
}

void gorc::gui::widget::on_mouse_down(const time&, const vector<2, int>&, sf::Mouse::Button) {
    return;
}

void gorc::gui::widget::on_mouse_up(const time&, const vector<2, int>&, sf::Mouse::Button) {
    return;
}

bool gorc::gui::widget::wants_keyboard_focus() const {
    return false;
}

void gorc::gui::widget::on_gain_keyboard_focus(const time&) {
    return;
}

void gorc::gui::widget::on_lost_keyboard_focus(const time&) {
    return;
}

void gorc::gui::widget::on_key_down(const time&, sf::Keyboard::Key, bool, bool, bool) {
    return;
}

void gorc::gui::widget::on_key_up(const time&, sf::Keyboard::Key, bool, bool, bool) {
    return;
}

void gorc::gui::widget::on_text_entered(const time&, char) {
    return;
}

void gorc::gui::widget::draw(const time&, graphics::gui_renderer&, int) const {
    return;
}
