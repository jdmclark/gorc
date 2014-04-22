#pragma once

#include "base/gui/widget.h"
#include <functional>

namespace gorc {
namespace gui {

class gui_view;

namespace widgets {

class input_block_panel : public widget {
private:
    std::function<void()> click_predicate;

public:
    input_block_panel(gui_view&, std::function<void()>&& click_predicate);

    virtual bool wants_mouse_input() const override;
    virtual void on_mouse_down(const time& time, const vector<2, int>& position, sf::Mouse::Button button) override;
};

}
}
}
