#include "file.h"

Gorc::IO::File::File(const boost::filesystem::path& fn) : ReadOnlyFile(fn) {
	return;
}

Gorc::IO::File::~File() {
	return;
}
