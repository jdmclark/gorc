#pragma once

#include "base/gui/widget.h"
#include "content/assets/bitmap.h"
#include <string>

namespace gorc {
namespace gui {
class gui_view;

namespace widgets {

class dynamic_image : public widget {
private:
    const content::assets::bitmap& sprite_sheet;
    box<2, int> graphics_size;
    int current_cel = 0;

public:
    dynamic_image(gui_view&, const content::assets::bitmap& sprite_sheet, const box<2, int>& graphics_size);

    virtual box<2, int> get_minimum_size(const gui_view&) const override;

    virtual void draw(const time& time, graphics::gui_renderer& renderer, int depth) const override;

    virtual void set_cel(int cel);
};

}
}
}
