#pragma once

#include <string>

namespace gorc {
namespace game {
namespace events {

class window_focus_event {
public:
	static const std::string& get_event_type();

	const bool has_focus;

	window_focus_event(bool HasFocus);
};

}
}
}
