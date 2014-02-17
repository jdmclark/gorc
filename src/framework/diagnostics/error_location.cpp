#include "error_location.h"
#include <cstring>

gorc::diagnostics::error_location::error_location()
    : filename("unknown"), first_line(1), first_column(1), last_line(1), last_column(1) {
    return;
}

gorc::diagnostics::error_location::error_location(const boost::filesystem::path& filename,
        unsigned int firstline, unsigned int firstcol, unsigned int lastline, unsigned int lastcol)
    : filename(filename), first_line(firstline), first_column(firstcol), last_line(lastline), last_column(lastcol) {
    return;
}

gorc::diagnostics::error_location::error_location(const error_location& loc) {
    *this = loc;
}

const gorc::diagnostics::error_location& gorc::diagnostics::error_location::operator=(const error_location& loc) {
    filename = loc.filename;
    first_line = loc.first_line;
    first_column = loc.first_column;
    last_line = loc.last_line;
    last_column = loc.last_column;
    return loc;
}

bool gorc::diagnostics::error_location::operator==(const error_location& l) const {
    return filename == l.filename && first_line == l.first_line && last_line == l.last_line
            && first_column == l.first_column && last_column == l.last_column;
}
