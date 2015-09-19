#pragma once

#include "io/path.hpp"
#include "io/input_stream.hpp"
#include <memory>

namespace gorc {

    class virtual_container;

    class virtual_file {
    public:
        path const name;

        virtual_file(path const &fn);
        virtual ~virtual_file();

        virtual std::unique_ptr<input_stream> open() const = 0;
        virtual virtual_container const& get_parent_container() const = 0;
    };
}
