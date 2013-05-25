#include "file.h"

Gorc::IO::File::File(const std::string& fn) : ReadOnlyFile(fn) {
	return;
}

Gorc::IO::File::~File() {
	return;
}
