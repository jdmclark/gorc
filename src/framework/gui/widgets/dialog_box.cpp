#include "dialog_box.h"
#include "framework/gui/gui_view.h"

gorc::gui::widgets::dialog_box::dialog_box(gui_view& v, const content::assets::texture& skin)
    : background(v.add_child<input_block_panel>(*this, []{})),
      panel(v.add_child<frame>(*this, skin)) {
    visible = false;
    zbias = 2000;
    panel.zbias = 2;
    panel.padding = 2;
    panel.horizontal_align = layout::horizontal_align_style::center;
    panel.vertical_align = layout::vertical_align_style::middle;
    dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };
    background.dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };

    return;
}
