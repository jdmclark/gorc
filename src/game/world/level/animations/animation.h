#pragma once

#include <memory>

namespace gorc {
namespace game {
namespace world {
namespace level {
namespace animations {

class animation {
public:
	bool expired = false;

	virtual ~animation();

	virtual int get_id() const;
	virtual void update(double dt) = 0;
	virtual void stop();
};

}
}
}
}
}
