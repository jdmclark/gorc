#pragma once

#include <string>

namespace test {

    class exception {
    public:
        std::string const filename;
        std::string const reason;
        const int line_number;

        exception(std::string const &filename,
                  int line_number,
                  std::string const &reason);
    };

}
