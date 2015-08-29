#pragma once

#include "options.hpp"
#include "utility/range.hpp"

namespace gorc {

    class program {
    private:
        options opts;

    protected:
        virtual void create_options(options &) = 0;
        virtual int main() = 0;

    public:
        program();
        int run(range<char**> const &);
    };

}

#define MAKE_MAIN(ProgramT)                                                             \
int main(int argc, char **argv) {                                                       \
    ProgramT program;                                                                   \
    return program.run(gorc::make_range(argv + 1, argv + argc));                        \
}
