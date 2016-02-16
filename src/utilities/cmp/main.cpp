#include "program/program.hpp"

namespace gorc {

    class cmp_program : public program {
    public:
        virtual void create_options(options &) override
        {
            return;
        }

        virtual int run() override
        {
            return EXIT_SUCCESS;
        }
    };

}

MAKE_MAIN(gorc::cmp_program)
