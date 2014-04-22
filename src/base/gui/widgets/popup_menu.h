#pragma once

#include "base/content/assets/font.h"
#include "base/content/assets/texture.h"
#include "base/gui/widget.h"
#include "base/gui/widgets/input_block_panel.h"
#include "base/gui/widgets/frame.h"
#include "base/gui/widgets/grid_panel.h"
#include <initializer_list>
#include <tuple>
#include <functional>
#include <string>
#include <vector>

namespace gorc {
namespace gui {

class gui_view;

namespace widgets {

class popup_menu : public widget {
private:
    const content::assets::texture& skin;
    const content::assets::font& font;
    vector<2, int> desired_position = make_vector(0, 0);
    input_block_panel& background;
    frame& menu;
    grid_panel& grid;

public:
    popup_menu(gui_view&, const content::assets::texture& skin, const content::assets::font& font);

    void add_item(gui_view& v, const std::string& label, std::function<void()>&& fn);

    void set_desired_position(gui_view& v, const vector<2, int>& pos);
    virtual box<2, int> get_child_position(const gui_view&, const widget& child) const override;
};

}
}
}
