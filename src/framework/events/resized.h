#pragma once

#include "framework/math/box.h"

namespace gorc {
namespace events {

class resized {
public:
	static const std::string& get_event_type();

	box<2, int> view_size;

	resized(const box<2, int>& view_size);
};

}
}
