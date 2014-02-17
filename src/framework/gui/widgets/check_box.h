#pragma once

#include "frame.h"
#include "static_text.h"
#include <functional>

namespace gorc {
namespace gui {
class gui_view;

namespace widgets {

class check_box : public widget {
private:
    const content::assets::texture& skin;
    bool is_depressed = false;

public:
    bool selected = false;

    check_box(gui_view&, const content::assets::texture& skin, const content::assets::font& font, const std::string& text);

    virtual bool wants_mouse_input() const override;

    virtual void on_mouse_out(const time& time) override;
    virtual void on_mouse_down(const time& time, const vector<2, int>& position, sf::Mouse::Button button) override;
    virtual void on_mouse_up(const time& time, const vector<2, int>& position, sf::Mouse::Button button) override;

    virtual void draw(const time& time, graphics::gui_renderer&, int depth) const override;
};

}
}
}
