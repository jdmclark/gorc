#pragma once

namespace Gorc {

template <typename T> class Id {
private:
	int index;

public:
	Id() : index(-1) {
		return;
	}

	explicit Id(int index) : index(index) {
		return;
	}

	explicit operator int() const {
		return index;
	}

	inline bool IsValid() const {
		return index >= 0;
	}

	inline bool operator==(const Id& id) const {
		return index == id.index;
	}

	inline bool operator!=(const Id& id) const {
		return index != id.index;
	}
};

}
