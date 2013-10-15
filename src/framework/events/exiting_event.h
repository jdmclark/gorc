#pragma once

#include <string>

namespace gorc {
namespace events {

class exiting_event {
public:
	bool cancel;

	exiting_event();

	static const std::string& get_event_type();
};

}
}
