#pragma once

#include "frame.h"
#include "static_text.h"
#include <functional>

namespace gorc {
namespace gui {
class gui_view;

namespace widgets {

class button : public widget {
private:
    frame* container_box;
    static_text* label;
    std::function<void()> click_predicate;
    bool is_depressed = false;

    frame_style style = frame_style::outset;
    frame_style depressed_style = frame_style::inset;

    inline void set_is_depressed(bool value) {
        is_depressed = value;
        container_box->style = (value) ? depressed_style : style;
    }

public:
    button(gui_view&, const content::assets::texture& skin, const content::assets::font& font, const std::string& text,
            const std::function<void()>& click_predicate);

    inline void set_style(frame_style style, frame_style depressed_style, layout::horizontal_align_style label_align) {
        this->style = style;
        this->depressed_style = depressed_style;

        set_is_depressed(is_depressed);

        label->horizontal_align = label_align;
    }

    virtual bool wants_mouse_input() const override;

    virtual void on_mouse_out(const time& time) override;
    virtual void on_mouse_down(const time& time, const vector<2, int>& position, sf::Mouse::Button button) override;
    virtual void on_mouse_up(const time& time, const vector<2, int>& position, sf::Mouse::Button button) override;
};

}
}
}
