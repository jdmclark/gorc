#pragma once

#include "framework/math/box.h"

namespace gorc {
namespace game {

class view {
public:
	virtual ~view();

	virtual void update(double dt) = 0;
	virtual void draw(double dt, const box<2, int>& view_size) = 0;
};

}
}
