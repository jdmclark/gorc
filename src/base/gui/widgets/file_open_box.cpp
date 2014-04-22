#include "file_open_box.h"
#include "base/gui/gui_view.h"

gorc::gui::widgets::file_open_box::file_open_box(gui_view& v, const content::assets::texture& skin, const content::assets::font& font)
    : dialog_box(v, skin),
      content_panel(v.add_child<grid_panel>(panel, skin, 64, 6, 20, 3, 4)),
      ok(v.add_child<button>(content_panel, skin, font, "OK", [this] { ok_clicked(); })),
      cancel(v.add_child<button>(content_panel, skin, font, "Cancel", [this] { cancel_clicked(); })),
      label(v.add_child<static_text>(content_panel, font, "Filename")),
      path(v.add_child<edit_box>(content_panel, skin, font, "")) {
    panel.padding = 4;

    content_panel.dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };

    auto& title = v.add_child<static_text>(content_panel, font, "Open File");
    title.horizontal_align = layout::horizontal_align_style::center;
    title.grid_position = make_vector(0, 0);
    title.column_span = 6;

    ok.grid_position = make_vector(4, 2);
    ok.dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };
    cancel.grid_position = make_vector(5, 2);
    cancel.dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };

    label.grid_position = make_vector(0, 1);
    label.vertical_align = layout::vertical_align_style::middle;

    path.grid_position = make_vector(1, 1);
    path.column_span = 5;
    path.dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };

    return;
}

void gorc::gui::widgets::file_open_box::ok_clicked() {
    visible = false;
    ok_p(path.get_text());
    return;
}

void gorc::gui::widgets::file_open_box::cancel_clicked() {
    visible = false;
    return;
}

void gorc::gui::widgets::file_open_box::show(std::function<void(const boost::filesystem::path&)>&& ok_p) {
    path.clear();
    this->ok_p = ok_p;
    visible = true;
}
