#include "mutual_exclusion.hpp"
#include "log/log.hpp"
#include "utility/oxford_join.hpp"
#include "options.hpp"

gorc::mutual_exclusion::mutual_exclusion(std::vector<std::string> const &opts,
                                         int min_set,
                                         int max_set)
    : exclusive_options(opts)
    , min_set(min_set)
    , max_set(max_set)
{
    return;
}

void gorc::mutual_exclusion::check_constraint(options const &opt) const
{
    int count = 0;
    for(auto const &exclusive_option : exclusive_options) {
        if(opt.has_value(exclusive_option)) {
            ++count;
        }
    }

    if(count < min_set) {
        LOG_FATAL(format("At least %d of %s must be specified") %
                  min_set %
                  oxford_join(exclusive_options));
    }
    else if(count > max_set) {
        LOG_FATAL(format("At most %d of %s may be specified") %
                  max_set %
                  oxford_join(exclusive_options));
    }
}
