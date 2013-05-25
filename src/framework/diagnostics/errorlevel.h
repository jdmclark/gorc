#pragma once

#include <iosfwd>

namespace Gorc {
namespace Diagnostics {

enum class ErrorLevel {
	Warning,
	Error,
	CriticalError
};

}
}

std::ostream& operator<<(std::ostream& str, Gorc::Diagnostics::ErrorLevel level);
