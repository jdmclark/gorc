#include "exception.hpp"

test::exception::exception(std::string const &filename,
                           int lineNumber,
                           std::string const &reason)
    : filename(filename)
    , reason(reason)
    , line_number(lineNumber) {
    return;
}
