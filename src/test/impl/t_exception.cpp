#include "t_exception.h"

test::exception::exception(const std::string& filename, const int lineNumber, const std::string& reason)
    : filename(filename), reason(reason), line_number(lineNumber) {
    return;
}
