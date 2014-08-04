#include "json_loader.h"
#include "base/io/input_stream.h"
#include "base/io/json_input_stream.h"

using namespace gorc::content;

std::unique_ptr<asset> json_loader::deserialize(boost::filesystem::path const &filename,
                                                io::read_only_file &file,
                                                content_manager &manager,
                                                diagnostics::report &report) {
    auto is = io::make_input_stream(file);
    auto jis = io::json_input_stream(is);

    return parse(filename, jis, manager, report);
}
