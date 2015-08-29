#pragma once

#include "abstract_argument_queue.hpp"
#include <string>

namespace gorc {

    class abstract_option {
    protected:
        abstract_option(std::string const &name);

    public:
        std::string const name;

        virtual ~abstract_option();

        virtual void reset() = 0;
        virtual void load_from_arg_list(abstract_argument_queue &arg_list) = 0;

        virtual bool has_value() const = 0;
    };

}
