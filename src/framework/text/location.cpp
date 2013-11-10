#include "location.h"
#include <cstring>

gorc::text::location::location()
	: filename(nullptr), first_line(1), first_column(1), last_line(1), last_column(1) {
	return;
}

gorc::text::location::location(const char* filename,
		unsigned int firstline, unsigned int firstcol, unsigned int lastline, unsigned int lastcol)
	: filename(filename), first_line(firstline), first_column(firstcol), last_line(lastline), last_column(lastcol) {
	return;
}

gorc::text::location::location(const location& loc) {
	*this = loc;
}

const gorc::text::location& gorc::text::location::operator=(const location& loc) {
	filename = loc.filename;
	first_line = loc.first_line;
	first_column = loc.first_column;
	last_line = loc.last_line;
	last_column = loc.last_column;
	return loc;
}

bool gorc::text::location::operator==(const location& l) const {
	return std::strcmp(filename, l.filename) == 0 && first_line == l.first_line && last_line == l.last_line
			&& first_column == l.first_column && last_column == l.last_column;
}

gorc::text::location::operator gorc::diagnostics::error_location() const {
	return diagnostics::error_location((filename != nullptr) ? filename : "unknown", first_line, first_column, last_line, last_column);
}
