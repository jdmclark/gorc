#pragma once

#include "base/gui/widget.h"
#include "base/content/assets/font.h"
#include <string>

namespace gorc {
namespace gui {
namespace widgets {

class wrapped_text : public widget {
private:
    const content::assets::font& font;

    void draw_range(std::string::const_iterator begin, std::string::const_iterator end, int left, int top,
            graphics::gui_renderer& renderer, int depth) const;

public:
    std::string text;

    wrapped_text(gui_view&, const content::assets::font& font, const std::string& text);

    virtual box<2, int> get_minimum_size(const gui_view&) const override;
    virtual void draw(const time& time, graphics::gui_renderer& renderer, int depth) const override;
};

}
}
}
