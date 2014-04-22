#pragma once

#include "base/content/assets/texture.h"
#include "base/gui/widget.h"
#include "base/gui/widgets/input_block_panel.h"
#include "base/gui/widgets/frame.h"

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
