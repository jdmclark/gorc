#pragma once

#include <string>

namespace gorc {

    std::string wrapped(std::string const &msg, size_t width = 80);
    std::string wrapped(std::string const &tag, std::string const &msg, size_t width = 80);

}
