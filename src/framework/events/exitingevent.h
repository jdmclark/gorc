#pragma once

#include <string>

namespace Gorc {
namespace Events {

class ExitingEvent {
public:
	bool Cancel;

	ExitingEvent();

	static const std::string& GetEventType();
};

}
}
