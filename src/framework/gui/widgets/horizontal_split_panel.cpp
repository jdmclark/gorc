#include "horizontal_split_panel.h"
#include "framework/gui/gui_view.h"

gorc::gui::widgets::horizontal_split_panel::horizontal_split_panel(gui_view& v)
	: top(v.add_child<gorc::gui::widgets::panel>(*this)), bottom(v.add_child<gorc::gui::widgets::panel>(*this)) {
	return;
}

gorc::box<2, int> gorc::gui::widgets::horizontal_split_panel::get_child_position(const gui_view& v, const widget& child) const {
	int left_pos, right_pos, top_pos, bottom_pos;
	std::tie(left_pos, right_pos) = get_range<0>(position);
	std::tie(top_pos, bottom_pos) = get_range<1>(position);

	left_pos += padding.left;
	top_pos += padding.top;
	bottom_pos -= padding.bottom;
	right_pos -= padding.right;

	int actual_split_pos;
	switch(style) {
	case horizontal_split_style::top_px:
		actual_split_pos = top_pos + static_cast<int>(split_position);
		break;

	case horizontal_split_style::bottom_px:
		actual_split_pos = bottom_pos - static_cast<int>(split_position);
		break;

	case horizontal_split_style::top_percent:
		actual_split_pos = static_cast<int>(lerp(static_cast<double>(top_pos), static_cast<double>(bottom_pos), split_position));
		break;

	case horizontal_split_style::bottom_percent:
		actual_split_pos = static_cast<int>(lerp(static_cast<double>(bottom_pos), static_cast<double>(top_pos), split_position));
		break;
	}

	actual_split_pos = clamp(actual_split_pos, top_pos, bottom_pos);

	if(&child == &top) {
		return make_box(make_vector(left_pos + child.margin.left, top_pos + child.margin.top),
				make_vector(right_pos - child.margin.right, actual_split_pos - child.margin.bottom));
	}
	else if(&child == &bottom) {
		return make_box(make_vector(left_pos + child.margin.left, actual_split_pos + child.margin.top),
				make_vector(right_pos - child.margin.right, bottom_pos - child.margin.bottom));
	}
	else {
		return widget::get_child_position(v, child);
	}
}
