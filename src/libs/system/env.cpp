#include "env.hpp"
#include <cstdlib>
#include <iostream>
#include <boost/format.hpp>

bool gorc::has_environment_variable(std::string const &key)
{
    return std::getenv(key.c_str()) != nullptr;
}

gorc::maybe<std::string> gorc::get_environment_variable(std::string const &key)
{
    char const *value = std::getenv(key.c_str());
    if(value) {
        return make_maybe<std::string>(value);
    }

    return nothing;
}

void gorc::set_environment_variable(std::string const &key,
                                    std::string const &value)
{
#ifdef PLATFORM_MINGW
    auto env_s = boost::str(boost::format("%s=%s") % key % value);
    _putenv(env_s.c_str());
#else
    if(value.empty()) {
        unsetenv(key.c_str());
    }
    else {
        setenv(key.c_str(), value.c_str(), 1);
    }
#endif
}
