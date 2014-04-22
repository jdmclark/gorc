#pragma once

#include "base/utility/time.h"
#include "base/math/box.h"
#include "base/input/input_adapter.h"
#include "base/utility/maybe.h"
#include "base/graphics/render_target.h"

namespace gorc {

class view {
public:
    virtual ~view();

    virtual void resize(const box<2, int>& view_size);
    virtual void draw(const time& time, const box<2, int>& view_size, graphics::render_target& target);
    virtual maybe<input::input_adapter*> get_input_adapter();
};

}
