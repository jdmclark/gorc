#include "vertical_split_panel.h"
#include "base/gui/gui_view.h"
#include "base/math/util.h"

gorc::gui::widgets::vertical_split_panel::vertical_split_panel(gui_view& v)
    : left(v.add_child<gorc::gui::widgets::panel>(*this)), right(v.add_child<gorc::gui::widgets::panel>(*this)) {
    return;
}

gorc::box<2, int> gorc::gui::widgets::vertical_split_panel::get_child_position(const gui_view& v, const widget& child) const {
    int left_pos, right_pos, top_pos, bottom_pos;
    std::tie(left_pos, right_pos) = get_range<0>(position);
    std::tie(top_pos, bottom_pos) = get_range<1>(position);

    left_pos += padding.left;
    top_pos += padding.top;
    bottom_pos -= padding.bottom;
    right_pos -= padding.right;

    int actual_split_pos = 0;
    switch(style) {
    case vertical_split_style::left_px:
        actual_split_pos = left_pos + static_cast<int>(split_position);
        break;

    case vertical_split_style::right_px:
        actual_split_pos = right_pos - static_cast<int>(split_position);
        break;

    case vertical_split_style::left_percent:
        actual_split_pos = static_cast<int>(lerp(static_cast<double>(left_pos), static_cast<double>(right_pos), split_position));
        break;

    case vertical_split_style::right_percent:
        actual_split_pos = static_cast<int>(lerp(static_cast<double>(right_pos), static_cast<double>(left_pos), split_position));
        break;
    }

    actual_split_pos = clamp(actual_split_pos, left_pos, right_pos);

    if(&child == &left) {
        return make_box(make_vector(left_pos + child.margin.left, top_pos + child.margin.top),
                make_vector(actual_split_pos - child.margin.right, bottom_pos - child.margin.bottom));
    }
    else if(&child == &right) {
        return make_box(make_vector(actual_split_pos + child.margin.left, top_pos + child.margin.top),
                make_vector(right_pos - child.margin.right, bottom_pos - child.margin.bottom));
    }
    else {
        return widget::get_child_position(v, child);
    }
}
