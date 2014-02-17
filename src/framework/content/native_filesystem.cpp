#include "native_filesystem.h"
#include "framework/io/native_file.h"

gorc::content::native_filesystem::native_filesystem(const boost::filesystem::path& basepath)
    : basepath(basepath) {
    return;
}

std::unique_ptr<gorc::io::read_only_file> gorc::content::native_filesystem::open(const boost::filesystem::path& file) const {
    return std::unique_ptr<gorc::io::read_only_file>(new gorc::io::native_file(basepath / file));
}
