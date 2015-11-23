#include "program/program.hpp"

namespace gorc {

    class boc_shell_program : public program {
    public:
        std::string script_filename;

        virtual void create_options(options &opts) override
        {
            opts.insert(make_value_option("script", script_filename));
            opts.emplace_constraint<required_option>("script");
            return;
        }

        virtual int main() override
        {
            return EXIT_SUCCESS;
        }
    };

}

MAKE_MAIN(gorc::boc_shell_program)
