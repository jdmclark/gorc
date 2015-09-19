#include "dependent_option.hpp"
#include "log/log.hpp"
#include "options.hpp"

gorc::dependent_option::dependent_option(std::string const &option,
                                         std::string const &dependency)
    : option(option)
    , dependency(dependency)
{
    return;
}

void gorc::dependent_option::check_constraint(options const &opt) const
{
    if(opt.has_value(option) &&
       !opt.has_value(dependency)) {
        LOG_FATAL(format("option %s requires option %d") %
                  option %
                  dependency);
    }
}
