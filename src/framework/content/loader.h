#pragma once

#include "framework/diagnostics/report.h"
#include "base/io/read_only_file.h"
#include "asset.h"
#include <memory>
#include <boost/filesystem/path.hpp>

namespace gorc {
namespace content {

class content_manager;

class loader {
public:
    virtual ~loader();

    virtual std::unique_ptr<asset> deserialize(const boost::filesystem::path& filename,
            io::read_only_file& file, content_manager& manager, diagnostics::report& report) = 0;
};

}
}
