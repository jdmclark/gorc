#pragma once

#include "io/path.hpp"
#include "io/read_only_file.hpp"
#include <memory>

namespace gorc {

    class virtual_file {
    public:
        path const name;

        virtual_file(path const &fn);
        virtual ~virtual_file();

        virtual std::unique_ptr<read_only_file> open() const = 0;
    };
}
