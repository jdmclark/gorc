#pragma once

#include <string>

namespace gorc {

    class abstract_argument_queue {
    public:
        virtual ~abstract_argument_queue();

        virtual bool empty() const = 0;
        virtual std::string peek() const = 0;
        virtual void pop() = 0;
    };

}
