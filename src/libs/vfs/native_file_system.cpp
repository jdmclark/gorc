#include "native_file_system.hpp"
#include "io/native_file.hpp"
#include "log/log.hpp"
#include <boost/filesystem.hpp>

gorc::native_file_system::native_file_system()
{
    return;
}

gorc::native_file_system::native_file_system(path const &base_path)
    : base_path(base_path)
{
    return;
}

std::unique_ptr<gorc::input_stream> gorc::native_file_system::open(path const &filename) const
{
    try {
        return make_native_read_only_file(base_path / filename);
    }
    catch(std::exception const &e) {
        std::string generic_fn = filename.generic_string();
        diagnostic_context dc(generic_fn.c_str());
        LOG_FATAL(e.what());
    }
}

std::tuple<gorc::path, std::unique_ptr<gorc::input_stream>>
    gorc::native_file_system::find(path const &filename,
                                   std::vector<path> const &prefixes) const
{
    for(auto const &prefix : prefixes) {
        path assembled = base_path / prefix / filename;
        if(boost::filesystem::is_regular_file(assembled)) {
            return make_tuple(assembled, make_native_read_only_file(assembled));
        }
    }

    // Check in the same directory
    return make_tuple(filename, open(filename));
}
