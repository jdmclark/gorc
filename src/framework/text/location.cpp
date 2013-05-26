#include "location.h"
#include <cstring>

Gorc::Text::Location::Location()
	: filename(nullptr), first_line(1), first_column(1), last_line(1), last_column(1) {
	return;
}

Gorc::Text::Location::Location(const char* filename,
		unsigned int firstline, unsigned int firstcol, unsigned int lastline, unsigned int lastcol)
	: filename(filename), first_line(firstline), first_column(firstcol), last_line(lastline), last_column(lastcol) {
	return;
}

Gorc::Text::Location::Location(const Location& loc) {
	*this = loc;
}

const Gorc::Text::Location& Gorc::Text::Location::operator=(const Location& loc) {
	filename = loc.filename;
	first_line = loc.first_line;
	first_column = loc.first_column;
	last_line = loc.last_line;
	last_column = loc.last_column;
	return loc;
}

bool Gorc::Text::Location::operator==(const Location& l) const {
	return std::strcmp(filename, l.filename) == 0 && first_line == l.first_line && last_line == l.last_line
			&& first_column == l.first_column && last_column == l.last_column;
}

Gorc::Text::Location::operator Gorc::Diagnostics::ErrorLocation() const {
	return Diagnostics::ErrorLocation((filename != nullptr) ? filename : "unknown", first_line, first_column, last_line, last_column);
}
