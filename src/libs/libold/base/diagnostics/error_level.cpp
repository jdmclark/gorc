#include <iostream>
#include "error_level.hpp"

std::ostream& operator<<(std::ostream& str, gorc::diagnostics::error_level value) {
    switch(value) {
    case gorc::diagnostics::error_level::warning:
        return str << "warning";

    case gorc::diagnostics::error_level::error:
        return str << "error";

    case gorc::diagnostics::error_level::critical_error:
        return str << "critical error";
    }

    return str;
}
