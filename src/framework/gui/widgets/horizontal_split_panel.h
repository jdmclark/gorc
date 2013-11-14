#pragma once

#include "panel.h"
#include "framework/gui/widget.h"
#include <string>

namespace gorc {
namespace gui {
class gui_view;

namespace widgets {

enum class horizontal_split_style {
	top_px,
	top_percent,
	bottom_px,
	bottom_percent
};

class horizontal_split_panel : public widget {
public:
	panel& top;
	panel& bottom;
	horizontal_split_style style = horizontal_split_style::top_percent;
	double split_position = 0.5;

	horizontal_split_panel(gui_view&);

	virtual box<2, int> get_child_position(const gui_view&, const widget& child) const override;
};

}
}
}
