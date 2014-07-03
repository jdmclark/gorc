#pragma once

#include "loader.h"
#include "base/io/json_input_stream.h"

namespace gorc {
namespace content {

class json_loader : public loader {
protected:
    virtual std::unique_ptr<asset> parse(boost::filesystem::path const &filename,
                                         io::json_input_stream &is,
                                         content_manager &manager,
                                         diagnostics::report &report) = 0;
public:
    virtual std::unique_ptr<asset> deserialize(boost::filesystem::path const &filename,
                                               io::read_only_file &file,
                                               content_manager &manager,
                                               diagnostics::report &report) override;
};

}
}
