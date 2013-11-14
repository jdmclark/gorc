#pragma once

#include "panel.h"
#include "framework/gui/widget.h"
#include "framework/gui/widgets/frame.h"
#include <string>

namespace gorc {
namespace gui {
namespace widgets {

class grid_panel : public widget {
private:
	const content::assets::texture& tex;
	int row_width;
	int n_rows;
	int col_width;
	int n_cols;
	int gutter_size;

public:
	bool draw_wireframe = false;

	grid_panel(gui_view&, const content::assets::texture& tex,
			int col_width, int n_cols, int row_width, int n_rows, int gutter_size);

	int add_row() {
		return n_rows++;
	}

	int add_col() {
		return n_cols++;
	}

	virtual box<2, int> get_minimum_size(const gui_view&) const override;
	virtual box<2, int> get_child_position(const gui_view&, const widget& child) const override;

	virtual void draw(const time& time, graphics::gui_renderer&, int depth) const override;
};

}
}
}
