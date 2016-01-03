#include "self.hpp"
#include <string>
#include <system_error>

#ifdef PLATFORM_LINUX

#include <limits.h>
#include <stdlib.h>

gorc::path gorc::get_self_executable_path()
{
    char const *proc_path = "/proc/self/exe";
    char buffer[PATH_MAX];

    char const *result = ::realpath(proc_path, buffer);

    // LCOV_EXCL_START
    if(result == nullptr) {
        throw std::system_error(errno, std::generic_category());
    }
    // LCOV_EXCL_STOP

    return result;
}

#endif
