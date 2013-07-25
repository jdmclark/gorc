#pragma once

#include <string>

namespace Gorc {
namespace Game {
namespace Events {

class WindowFocusEvent {
public:
	static const std::string& GetEventType();

	const bool HasFocus;

	WindowFocusEvent(bool HasFocus);
};

}
}
}
