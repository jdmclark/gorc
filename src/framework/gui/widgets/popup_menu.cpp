#include "popup_menu.h"
#include "framework/gui/gui_view.h"
#include "framework/gui/widgets/button.h"
#include "framework/gui/widgets/static_text.h"

gorc::gui::widgets::popup_menu::popup_menu(gui_view& v, const content::assets::texture& skin, const content::assets::font& font)
    : skin(skin), font(font),
      background(v.add_child<input_block_panel>(*this, [this]() { visible = false; })), menu(v.add_child<frame>(*this, skin)),
      grid(v.add_child<grid_panel>(menu, skin, 128, 1, 20, 0, 0)) {
    visible = false;
    zbias = 1000;
    menu.zbias = 2;
    menu.padding = 2;

    dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };
    background.dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };

    return;
}

void gorc::gui::widgets::popup_menu::add_item(gui_view& v, const std::string& label, std::function<void()>&& fn) {
    int new_row = grid.add_row();
    auto& btn = v.add_child<button>(grid, skin, font, label, [this, fn] { fn(); visible = false; });
    btn.dock_style = flag_set<layout::dock_style> { layout::dock_style::width };
    btn.set_style(frame_style::flat, frame_style::inset, layout::horizontal_align_style::left);
    btn.grid_position = make_vector(0, new_row);
}

void gorc::gui::widgets::popup_menu::set_desired_position(gui_view& v, const vector<2, int>& pos) {
    desired_position = pos;
    v.layout();
}

gorc::box<2, int> gorc::gui::widgets::popup_menu::get_child_position(const gui_view& v, const widget& child) const {
    if(&child == &menu) {
        // Compute optimal screen position from desired position.
        auto min_size = child.get_minimum_size(v);

        int left, right, top, bottom;
        std::tie(left, right) = get_range<0>(position);
        std::tie(top, bottom) = get_range<1>(position);

        auto x_pos = clamp(get<0>(desired_position), left, right - get_size<0>(min_size));
        auto y_pos = clamp(get<1>(desired_position), top, bottom - get_size<1>(min_size));

        return make_box(make_vector(x_pos, y_pos), make_vector(x_pos + get_size<0>(min_size), y_pos + get_size<1>(min_size)));
    }
    else {
        return widget::get_child_position(v, child);
    }
}
