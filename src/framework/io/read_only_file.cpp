#include "read_only_file.h"

gorc::io::read_only_file::read_only_file(const boost::filesystem::path& fn) : Filename(fn) {
    return;
}

gorc::io::read_only_file::~read_only_file() {
    return;
}
