#pragma once

#include <string>

namespace gorc {
namespace events {

class exiting {
public:
	bool cancel = false;

	static const std::string& get_event_type();
};

}
}
