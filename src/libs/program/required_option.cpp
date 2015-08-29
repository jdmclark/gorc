#include "required_option.hpp"
#include "log/log.hpp"
#include "options.hpp"
#include "utility/oxford_join.hpp"

gorc::required_option::required_option(std::string const &name)
{
    names.push_back(name);
    return;
}

gorc::required_option::required_option(std::vector<std::string> const &names)
    : names(names)
{
    return;
}

void gorc::required_option::check_constraint(options const &opt) const
{
    bool all_specified = true;
    for(auto const &name : names) {
        all_specified = all_specified && opt.has_value(name);
    }

    if(!all_specified) {
        LOG_FATAL(format("%s %s must be specified") %
                  ((names.size() > 1) ? "Options" : "Option") %
                  oxford_join(names));
    }
}
