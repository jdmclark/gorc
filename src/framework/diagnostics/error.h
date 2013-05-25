#pragma once

#include "errorlevel.h"
#include "errorlocation.h"

#include <string>

namespace Gorc {
namespace Diagnostics {

class Error {
public:
	const ErrorLevel Level;
	const std::string Stage;
	const std::string Reason;
	const ErrorLocation Location;

	Error(ErrorLevel level, const std::string& stage, const std::string& reason, const ErrorLocation& errorLocation);

	operator std::string() const;
};

}
}
