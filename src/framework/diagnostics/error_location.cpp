#include "error_location.h"
#include <cstring>

Gorc::Diagnostics::ErrorLocation::ErrorLocation()
	: filename("unknown"), first_line(1), first_column(1), last_line(1), last_column(1) {
	return;
}

Gorc::Diagnostics::ErrorLocation::ErrorLocation(const boost::filesystem::path& filename,
		unsigned int firstline, unsigned int firstcol, unsigned int lastline, unsigned int lastcol)
	: filename(filename), first_line(firstline), first_column(firstcol), last_line(lastline), last_column(lastcol) {
	return;
}

Gorc::Diagnostics::ErrorLocation::ErrorLocation(const ErrorLocation& loc) {
	*this = loc;
}

const Gorc::Diagnostics::ErrorLocation& Gorc::Diagnostics::ErrorLocation::operator=(const ErrorLocation& loc) {
	filename = loc.filename;
	first_line = loc.first_line;
	first_column = loc.first_column;
	last_line = loc.last_line;
	last_column = loc.last_column;
	return loc;
}

bool Gorc::Diagnostics::ErrorLocation::operator==(const ErrorLocation& l) const {
	return filename == l.filename && first_line == l.first_line && last_line == l.last_line
			&& first_column == l.first_column && last_column == l.last_column;
}
