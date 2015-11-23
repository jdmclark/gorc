#include "program/program.hpp"

namespace gorc {

    class boc_test_program : public program {
    public:
        virtual void create_options(options &) override
        {
            return;
        }

        virtual int main() override
        {
            return EXIT_SUCCESS;
        }
    };

}

MAKE_MAIN(gorc::boc_test_program)
