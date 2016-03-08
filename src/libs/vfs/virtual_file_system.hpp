#pragma once

#include <string>
#include <memory>
#include <vector>
#include <tuple>
#include "io/path.hpp"
#include "io/input_stream.hpp"

namespace gorc {

    class virtual_file_system {
    public:
        virtual ~virtual_file_system();

        virtual std::unique_ptr<input_stream> open(path const &filename) const = 0;

        virtual std::tuple<path, std::unique_ptr<input_stream>>
            find(path const &filename, std::vector<path> const &prefixes) const = 0;
    };

}
