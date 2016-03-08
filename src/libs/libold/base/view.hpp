#pragma once

#include "libold/base/utility/time.hpp"
#include "math/box.hpp"
#include "libold/base/input/input_adapter.hpp"
#include "utility/maybe.hpp"
#include "libold/base/graphics/render_target.hpp"

namespace gorc {

class view {
public:
    virtual ~view();

    virtual void resize(const box<2, int>& view_size);
    virtual void draw(const gorc::time& time, const box<2, int>& view_size, graphics::render_target& target);
    virtual maybe<input::input_adapter*> get_input_adapter();
};

}
