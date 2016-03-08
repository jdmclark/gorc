#pragma once

#include <iosfwd>

namespace gorc {
namespace diagnostics {

enum class error_level {
    warning,
    error,
    critical_error
};

}
}

std::ostream& operator<<(std::ostream& str, gorc::diagnostics::error_level level);
