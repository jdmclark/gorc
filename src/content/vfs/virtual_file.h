#pragma once

#include "base/io/read_only_file.h"
#include <boost/filesystem/path.hpp>
#include <memory>

namespace gorc {
namespace content {
namespace vfs {

class virtual_file {
public:
    virtual ~virtual_file();

    virtual const boost::filesystem::path& get_filename() const = 0;
    virtual std::unique_ptr<io::read_only_file> open() const = 0;
};

}
}
}
