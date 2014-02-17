#pragma once

#include "framework/diagnostics/report.h"
#include "framework/io/read_only_file.h"
#include "asset.h"
#include <memory>

namespace gorc {
namespace content {

class manager;

class loader {
public:
    virtual ~loader();

    virtual std::unique_ptr<asset> deserialize(io::read_only_file& file, manager& manager, diagnostics::report& report) = 0;
};

}
}
