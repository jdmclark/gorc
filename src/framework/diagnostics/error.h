#pragma once

#include "error_level.h"
#include "error_location.h"

#include <string>

namespace gorc {
namespace diagnostics {

class error {
public:
	const error_level level;
	const std::string stage;
	const std::string reason;
	const error_location location;

	error(error_level level, const std::string& stage, const std::string& reason, const error_location& errorLocation);

	operator std::string() const;
};

}
}
