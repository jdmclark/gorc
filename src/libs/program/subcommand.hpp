#pragma once

#include "options.hpp"
#include "abstract_argument_queue.hpp"

namespace gorc {

    class subcommand {
    private:
        options opts;

    protected:
        virtual void create_options(options &) = 0;
        virtual int main() = 0;

    public:
        virtual ~subcommand();

        int run(abstract_argument_queue &);
    };

}
