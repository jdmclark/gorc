#include <algorithm>
#include <iostream>
#include "program/program.hpp"
#include "io/std_input_stream.hpp"

class child_program : public gorc::program {
public:
    bool fail = false;
    bool failmsg = false;

    virtual void create_options(gorc::options &opts) override
    {
        opts.insert(gorc::make_switch_option("fail", fail));
        opts.insert(gorc::make_switch_option("failmsg", failmsg));
    }

    virtual int run() override
    {
        gorc::std_input_stream std_input_stream;

        std::string data;

        while(!std_input_stream.at_end()) {
            char buf[2048];
            size_t amt = std_input_stream.read_some(buf, 2048);
            std::cerr << "READ " << amt << " BYTES" << std::endl;
            if(amt > 0) {
                data.append(buf, amt);
            }
        }

        std::transform(data.begin(), data.end(), data.begin(), toupper);
        std::cout << data << std::endl;

        if(fail) {
            if(failmsg) {
                std::cerr << "SOME ERROR HAPPENED" << std::endl;
            }
            return EXIT_FAILURE;
        }
        else {
            return EXIT_SUCCESS;
        }
    }
};

MAKE_MAIN(child_program)
