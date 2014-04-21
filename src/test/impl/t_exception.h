#pragma once

#include <string>

namespace test {

class exception {
public:
    const std::string filename, reason;
    const int line_number;

    exception(const std::string& filename, const int line_number, const std::string& reason);
};

}
