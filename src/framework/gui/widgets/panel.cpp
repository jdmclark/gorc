#include "panel.h"

gorc::gui::widgets::panel::panel(gui_view&) {
    dock_style = flag_set<layout::dock_style> { layout::dock_style::fill };
    return;
}
