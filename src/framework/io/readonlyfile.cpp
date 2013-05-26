#include "readonlyfile.h"

Gorc::IO::ReadOnlyFile::ReadOnlyFile(const boost::filesystem::path& fn) : Filename(fn) {
	return;
}

Gorc::IO::ReadOnlyFile::~ReadOnlyFile() {
	return;
}
