#pragma once

#include <string>

namespace Gorc {
namespace Events {

class PrintEvent {
public:
	PrintEvent(const std::string& message);

	std::string message;

	static const std::string& GetEventType();
};

}
}
