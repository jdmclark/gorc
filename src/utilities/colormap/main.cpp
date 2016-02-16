#include "program/program.hpp"
#include "jkcontent/colormap.hpp"
#include "io/binary_input_stream.hpp"
#include "text/json_output_stream.hpp"
#include "io/std_input_stream.hpp"
#include "io/std_output_stream.hpp"
#include "log/log.hpp"

namespace gorc {

    class cmp_program : public program {
    public:
        virtual void create_options(options &) override
        {
            return;
        }

        virtual int run() override
        {
            diagnostic_context dc("<stdin>");

            std_input_stream is;
            is.reopen_as_binary();

            binary_input_stream bis(is);

            colormap cmp(deserialization_constructor, bis);

            std_output_stream os;
            json_output_stream jos(os);

            json_serialize(jos, cmp);
            return EXIT_SUCCESS;
        }
    };

}

MAKE_MAIN(gorc::cmp_program)
