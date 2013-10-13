#pragma once

#include <string>

namespace Gorc {
namespace Events {

class ShutdownEvent {
public:
	static const std::string& GetEventType();
};

}
}
