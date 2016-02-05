#pragma once

#include "options.hpp"
#include "abstract_argument_queue.hpp"

namespace gorc {

    class subcommand {
    private:
        options opts;

    protected:
        virtual void create_options(options &) = 0;
        virtual int run() = 0;

    public:
        virtual ~subcommand();

        int start(abstract_argument_queue &);
    };

}
