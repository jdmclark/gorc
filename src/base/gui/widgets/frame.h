#pragma once

#include "base/gui/widget.h"
#include "base/content/assets/texture.h"
#include <string>

namespace gorc {
namespace gui {
class gui_view;

namespace widgets {

enum class frame_style {
    flat = 0,
    outset = 1,
    inset = 2,
    border = 3,
};

class frame : public widget {
private:
    const content::assets::texture& sprite_sheet;

public:
    frame_style style = frame_style::outset;

    frame(gui_view&, const content::assets::texture& sprite_sheet);

    virtual void draw(const time& time, graphics::gui_renderer& renderer, int depth) const override;
};

}
}
}
