#pragma once

#include "abstract_argument_queue.hpp"
#include <string>

namespace gorc {

    class abstract_bare_option {
    protected:
        abstract_bare_option();

    public:
        virtual ~abstract_bare_option();

        virtual void reset() = 0;
        virtual void load_from_arg(std::string const &arg,
                                   abstract_argument_queue &arg_list) = 0;

        virtual bool has_value() const = 0;
    };

}
