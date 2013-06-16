#pragma once

#include "framework/math/box.h"

namespace Gorc {
namespace Game {

class View {
public:
	virtual ~View();

	virtual void Update(double dt) = 0;
	virtual void Draw(double dt, const Math::Box<2, unsigned int>& view_size) = 0;
};

}
}
