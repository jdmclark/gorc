#include "fourcc.hpp"
#include "libold/base/exception.hpp"
#include "log/log.hpp"

gorc::utility::fourcc::fourcc(const char* in_c) {
    int i = 0;
    for(; in_c[i]; ++i) {
        if(i >= 4) {
            LOG_FATAL(format("string '%s' is not a valid fourcc") % in_c);
        }

        code[i] = in_c[i];
    }

    for(; i < 4; ++i) {
        code[i] = '\0';
    }
}
