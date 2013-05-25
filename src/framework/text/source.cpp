#include "source.h"

Gorc::Text::Source::Source(Gorc::IO::ReadOnlyFile& file)
	: index(0), Filename(file.Filename) {
	size_t fsize = file.GetSize();
	if(fsize == 0) {
		return;
	}

	buffer.resize(fsize, 0);
	file.Read(&buffer[0], fsize);
}

Gorc::Text::Source::Source(const std::string& str)
	: index(0), Filename("internal buffer") {
	buffer.assign(str.begin(), str.end());
	return;
}
