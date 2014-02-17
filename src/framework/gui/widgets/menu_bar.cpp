#include "menu_bar.h"
#include "framework/gui/gui_view.h"
#include "framework/gui/widgets/button.h"
#include "framework/gui/widgets/static_text.h"
#include "framework/gui/widgets/popup_menu.h"

gorc::gui::widgets::menu_bar::menu_bar(gui_view& v, const content::assets::texture& skin, const content::assets::font& font)
    : horizontal_split_panel(v), skin(skin), font(font), menu(v.add_child<frame>(top, skin)),
      grid(v.add_child<grid_panel>(menu, skin, 64, 0, 20, 1, 0)) {
    style = horizontal_split_style::top_px;
    split_position = 22;

    menu.zbias = 2;
    menu.padding = 2;

    dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };
    menu.dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };

    return;
}

gorc::gui::widgets::popup_menu& gorc::gui::widgets::menu_bar::add_item(gui_view& v, const std::string& label) {
    int new_col = grid.add_col();
    auto& menu = v.add_child<popup_menu>(*this, skin, font);

    auto& btn = v.add_child<button>(grid, skin, font, label, [this, new_col, &menu, &v] {
        int left, bottom;
        std::tie(left, std::ignore) = get_range<0>(grid.position);
        std::tie(std::ignore, bottom) = get_range<1>(grid.position);
        menu.set_desired_position(v, make_vector(left + 64 * new_col, bottom));
        menu.visible = true;
    });

    btn.set_style(frame_style::flat, frame_style::inset, layout::horizontal_align_style::center);
    btn.grid_position = make_vector(new_col, 0);
    btn.dock_style = flag_set<layout::dock_style> { layout::dock_style::width };

    return menu;
}

/*void gorc::gui::widgets::menu_bar::add_item(gui_view& v, const std::string& label, std::function<void()>&& fn) {
    int new_row = grid.add_row();
    auto& btn = v.add_child<button>(grid, skin, font, label, [this, fn] { fn(); visible = false; });
    btn.dock_style = flag_set<layout::dock_style> { layout::dock_style::width };
    btn.set_style(frame_style::flat, frame_style::inset);
    btn.grid_position = make_vector(0, new_row);
}*/
