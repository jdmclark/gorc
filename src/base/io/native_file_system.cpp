#include "native_file_system.h"
#include "native_file.h"

gorc::io::native_file_system::native_file_system(const boost::filesystem::path& base_path) : base_path(base_path) {
    return;
}

std::unique_ptr<gorc::io::read_only_file> gorc::io::native_file_system::open(const boost::filesystem::path& filename) const {
    return make_native_read_only_file(base_path / filename);
}
