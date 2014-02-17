#pragma once

#include "panel.h"
#include "framework/gui/widget.h"
#include <string>

namespace gorc {
namespace gui {
class gui_view;

namespace widgets {

enum class vertical_split_style {
    left_px,
    left_percent,
    right_px,
    right_percent,
};

class vertical_split_panel : public widget {
public:
    panel& left;
    panel& right;
    vertical_split_style style = vertical_split_style::left_percent;
    double split_position = 0.5;

    vertical_split_panel(gui_view&);

    virtual box<2, int> get_child_position(const gui_view&, const widget& child) const override;
};

}
}
}
