#pragma once

namespace gorc {
namespace gui {
namespace layout {

class margin {
public:
	int left = 0;
	int right = 0;
	int top = 0;
	int bottom = 0;

	inline const margin& operator=(int v) {
		left = right = top = bottom = v;
		return *this;
	}
};

}
}
}
