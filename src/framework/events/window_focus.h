#pragma once

#include <string>

namespace gorc {
namespace events {

class window_focus {
public:
	static const std::string& get_event_type();

	const bool has_focus;

	window_focus(bool HasFocus);
};

}
}
