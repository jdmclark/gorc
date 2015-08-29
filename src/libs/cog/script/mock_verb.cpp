#include "mock_verb.hpp"
#include "log/log.hpp"

gorc::cog::value gorc::cog::mock_verb::invoke(stack &) const
{
    LOG_FATAL(format("invoked mock verb '%s'") % name);
    return value();
}
