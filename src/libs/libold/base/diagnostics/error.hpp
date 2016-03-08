#pragma once

#include "error_level.hpp"
#include "error_location.hpp"

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
