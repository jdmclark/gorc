#include "message_box.h"
#include "base/gui/gui_view.h"

gorc::gui::widgets::message_box::message_box(gui_view& v, const content::assets::texture& skin, const content::assets::font& font)
    : dialog_box(v, skin),
      content_panel(v.add_child<grid_panel>(panel, skin, 64, 6, 20, 4, 4)),
      ok(v.add_child<button>(content_panel, skin, font, "OK", [this] { ok_clicked(); })),
      cancel(v.add_child<button>(content_panel, skin, font, "Cancel", [this] { cancel_clicked(); })),
      ok_message(v.add_child<button>(content_panel, skin, font, "OK", [this] { ok_message_clicked(); })),
      message(v.add_child<wrapped_text>(content_panel, font, "MESSAGEBOX_TEXT")) {
    panel.padding = 4;
    content_panel.dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };

    message.grid_position = make_vector(0, 0);
    message.column_span = 6;
    message.row_span = 3;
    message.dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };

    ok.grid_position = make_vector(4, 3);
    ok.dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };
    cancel.grid_position = make_vector(5, 3);
    cancel.dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };
    ok_message.grid_position = make_vector(5, 3);
    ok_message.dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };

    return;
}

void gorc::gui::widgets::message_box::hide_all_buttons() {
    ok.visible = false;
    cancel.visible = false;

    ok_message.visible = false;
}

void gorc::gui::widgets::message_box::ok_clicked() {
    visible = false;
    ok_p();
    return;
}

void gorc::gui::widgets::message_box::cancel_clicked() {
    visible = false;
    cancel_p();
    return;
}

void gorc::gui::widgets::message_box::ok_message_clicked() {
    visible = false;
    return;
}

void gorc::gui::widgets::message_box::show_confirm(const std::string& message,
        std::function<void()>&& ok_p, std::function<void()>&& cancel_p) {
    this->message.text = message;
    this->ok_p = ok_p;
    this->cancel_p = cancel_p;

    hide_all_buttons();
    ok.visible = true;
    cancel.visible = true;
    visible = true;
}

void gorc::gui::widgets::message_box::show_message(const std::string& message) {
    this->message.text = message;

    hide_all_buttons();
    ok_message.visible = true;
    visible = true;
}
