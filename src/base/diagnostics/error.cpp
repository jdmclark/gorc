#include "error.h"

#include <sstream>
#include <iomanip>

using namespace gorc::diagnostics;

error::error(error_level level, const std::string& stage, const std::string& reason,
        const error_location& location)
    : level(level), stage(stage), reason(reason), location(location) {
    return;
}

std::string FormatError(const error& error) {
    std::stringstream ss;

    ss << error.level << ": " << error.reason << " [" << error.stage << "]";
    return ss.str();
}

std::string FormatLineError(const error& error) {
    std::stringstream ss;

    ss << error.location.filename << "(" << error.location.first_line << "," << error.location.first_column << "): "
            << error.level << ": " << error.reason << " [" << error.stage << "]";
    return ss.str();
}

error::operator std::string() const {
    switch(level) {
    case error_level::critical_error:
        return FormatError(*this);

    case error_level::error:
    case error_level::warning:
        return FormatLineError(*this);
    }

    return std::string();
}
