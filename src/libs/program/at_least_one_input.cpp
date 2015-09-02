#include "at_least_one_input.hpp"
#include "log/log.hpp"
#include "options.hpp"

void gorc::at_least_one_input::check_constraint(options const &opts) const
{
    if(!opts.has_bare_value()) {
        LOG_FATAL("No input files specified");
    }
}
