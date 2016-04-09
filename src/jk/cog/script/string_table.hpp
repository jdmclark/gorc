#pragma once

#include <string>
#include <vector>
#include <memory>

namespace gorc {
    namespace cog {

        class string_table {
        private:
            std::vector<std::unique_ptr<std::string>> strings;

        public:
            char const* add_string(std::string const &str);
        };

    }
}
