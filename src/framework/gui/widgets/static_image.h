#pragma once

#include "framework/gui/widget.h"
#include "framework/content/assets/texture.h"
#include <string>

namespace gorc {
namespace gui {
class gui_view;

namespace widgets {

class static_image : public widget {
private:
    const content::assets::texture& sprite_sheet;
    box<2, int> graphics_size;

public:
    static_image(gui_view&, const content::assets::texture& sprite_sheet, const box<2, int>& graphics_size);

    virtual box<2, int> get_minimum_size(const gui_view&) const override;

    virtual void draw(const time& time, graphics::gui_renderer& renderer, int depth) const override;
};

}
}
}
