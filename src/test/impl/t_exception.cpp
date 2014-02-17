#include "t_exception.h"

test::exception::exception(const std::string& filename, const int lineNumber, const std::string& reason)
    : filename(filename), reason(reason), line_number(lineNumber) {
    return;
}

test::internal_exception::internal_exception(const std::string& reason) : reason(reason) {
    return;
}
