#pragma once

#include <string>

namespace gorc {
namespace events {

class shutdown_event {
public:
	static const std::string& get_event_type();
};

}
}
