#pragma once

#include <boost/filesystem/path.hpp>
#include "content/vfs/virtual_file.h"

namespace gorc {
namespace content {
namespace vfs {
namespace gob {

class gob_container;

class gob_virtual_file : public virtual_file {
private:
    size_t chunkOffset;
    size_t chunkLength;
    boost::filesystem::path filename;
    const gob_container& parentContainer;

public:
    gob_virtual_file(size_t chunkOffset, size_t chunkLength, const boost::filesystem::path& filename, const gob_container& parentContainer);

    const boost::filesystem::path& get_filename() const;
    std::unique_ptr<io::read_only_file> open() const;
};

}
}
}
}
