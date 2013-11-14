#pragma once

#include "framework/content/assets/texture.h"
#include "framework/gui/widget.h"
#include "framework/gui/widgets/input_block_panel.h"
#include "framework/gui/widgets/frame.h"

namespace gorc {
namespace gui {

class gui_view;

namespace widgets {

class dialog_box : public widget {
private:
	input_block_panel& background;

protected:
	frame& panel;

	dialog_box(gui_view&, const content::assets::texture& skin);
};

}
}
}
