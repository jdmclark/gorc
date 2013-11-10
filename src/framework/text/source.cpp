#include "source.h"

gorc::text::source::source(gorc::io::read_only_file& file)
	: index(0), filename(file.Filename.generic_string()) {
	size_t fsize = file.get_size();
	if(fsize == 0) {
		return;
	}

	buffer.resize(fsize, 0);
	file.read(&buffer[0], fsize);
}

gorc::text::source::source(const std::string& str)
	: index(0), filename("internal buffer") {
	buffer.assign(str.begin(), str.end());
	return;
}
