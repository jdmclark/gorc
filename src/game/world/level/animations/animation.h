#pragma once

#include "framework/id.h"
#include <memory>

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Animations {

class Animation {
public:
	bool Expired = false;

	virtual ~Animation();

	virtual void Update(double dt) = 0;
	virtual void Stop();
};

}
}
}
}
}
