#include "read_only_file.h"

Gorc::IO::ReadOnlyFile::ReadOnlyFile(const boost::filesystem::path& fn) : Filename(fn) {
	return;
}

Gorc::IO::ReadOnlyFile::~ReadOnlyFile() {
	return;
}
