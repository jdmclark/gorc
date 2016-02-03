#include "native_file_system.hpp"
#include "io/native_file.hpp"
#include "log/log.hpp"

std::unique_ptr<gorc::input_stream> gorc::native_file_system::open(path const &filename) const
{
    try {
        return make_native_read_only_file(filename);
    }
    catch(std::exception const &e) {
        LOG_FATAL(e.what());
    }
}
