#pragma once

#include "libold/base/view.hpp"

namespace gorc {
namespace client {

class clear_screen_view : public gorc::view {
public:
    virtual void draw(const gorc::time& time, const box<2, int>& view_size, graphics::render_target&) override;
};

}
}
