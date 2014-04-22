#pragma once

#include "base/gui/widgets/dialog_box.h"
#include "base/gui/widgets/grid_panel.h"
#include "base/gui/widgets/wrapped_text.h"
#include "base/gui/widgets/button.h"

namespace gorc {
namespace gui {

class gui_view;

namespace widgets {

class message_box : public dialog_box {
private:
    grid_panel& content_panel;
    button &ok, &cancel;
    button &ok_message;
    wrapped_text& message;
    std::function<void()> ok_p, cancel_p;

    void hide_all_buttons();

    void ok_clicked();
    void cancel_clicked();
    void ok_message_clicked();

public:
    void show_confirm(const std::string& message, std::function<void()>&& ok_p, std::function<void()>&& cancel_p);
    void show_message(const std::string& message);

    message_box(gui_view&, const content::assets::texture& skin, const content::assets::font& font);
};

}
}
}
