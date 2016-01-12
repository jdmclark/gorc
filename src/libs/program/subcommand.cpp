#include "subcommand.hpp"

gorc::subcommand::~subcommand()
{
    return;
}

int gorc::subcommand::run(abstract_argument_queue &args)
{
    create_options(opts);
    opts.load_from_arg_queue(args);

    return main();
}
