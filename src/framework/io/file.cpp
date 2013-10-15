#include "file.h"

gorc::io::file::file(const boost::filesystem::path& fn) : read_only_file(fn) {
	return;
}

gorc::io::file::~file() {
	return;
}
