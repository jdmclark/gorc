#pragma once

#include <string>

namespace gorc {
namespace events {

class exit {
public:
	static const std::string& get_event_type();
};

}
}
