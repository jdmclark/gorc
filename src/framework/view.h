#pragma once

#include "framework/utility/time.h"
#include "framework/math/box.h"
#include "framework/input/input_adapter.h"
#include "framework/utility/maybe.h"
#include "framework/graphics/render_target.h"

namespace gorc {

class view {
public:
	virtual ~view();

	virtual void resize(const box<2, int>& view_size);
	virtual void draw(const time& time, const box<2, int>& view_size, graphics::render_target& target);
	virtual maybe<input::input_adapter*> get_input_adapter();
};

}
