#pragma once

#include "base/content/assets/font.h"
#include "base/content/assets/texture.h"
#include "base/gui/widget.h"
#include "base/gui/widgets/input_block_panel.h"
#include "base/gui/widgets/horizontal_split_panel.h"
#include "base/gui/widgets/frame.h"
#include "base/gui/widgets/grid_panel.h"
#include "base/gui/widgets/popup_menu.h"
#include <initializer_list>
#include <tuple>
#include <functional>
#include <string>
#include <vector>

namespace gorc {
namespace gui {

class gui_view;

namespace widgets {

class menu_bar : public horizontal_split_panel {
private:
    const content::assets::texture& skin;
    const content::assets::font& font;
    vector<2, int> desired_position = make_vector(0, 0);
    frame& menu;
    grid_panel& grid;

public:
    menu_bar(gui_view&, const content::assets::texture& skin, const content::assets::font& font);

    popup_menu& add_item(gui_view& v, const std::string& label);
};

}
}
}
