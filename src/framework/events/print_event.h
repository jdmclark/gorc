#pragma once

#include <string>

namespace gorc {
namespace events {

class print_event {
public:
	print_event(const std::string& message);

	std::string message;

	static const std::string& get_event_type();
};

}
}
