#include "value.hpp"
#include "utility/join.hpp"
#include "log/log.hpp"

gorc::shvalue gorc::shvalue_from_string(std::string const &str)
{
    return shvalue({ str });
}

std::string gorc::shvalue_to_string(gorc::shvalue const &sv)
{
    return join(sv, " ");
}

gorc::shvalue gorc::shvalue_from_boolean(bool v)
{
    return shvalue({ (v ? "true" : "false" ) });
}

bool gorc::shvalue_to_boolean(shvalue const &sv)
{
    if(sv.size() != 1) {
        LOG_FATAL("expected boolean, found list");
    }

    if(sv.front() == "true") {
        return true;
    }
    else if(sv.front() == "false") {
        return false;
    }
    else {
        LOG_FATAL(format("expected boolean, found '%s'") % sv.front());
    }
}
