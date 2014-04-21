#pragma once

#include <string>
#include <memory>
#include <boost/filesystem/path.hpp>
#include "read_only_file.h"

namespace gorc {
namespace io {

class file_system {
public:
    virtual ~file_system();

    virtual std::unique_ptr<read_only_file> open(const boost::filesystem::path& filename) const = 0;
};

}
}
