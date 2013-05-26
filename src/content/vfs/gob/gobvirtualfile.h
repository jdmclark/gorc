#pragma once

#include <boost/filesystem/path.hpp>
#include "content/vfs/virtualfile.h"

namespace Gorc {
namespace Content {
namespace VFS {
namespace Gob {

class GobContainer;

class GobVirtualFile : public VirtualFile {
private:
	size_t chunkOffset;
	size_t chunkLength;
	boost::filesystem::path filename;
	const GobContainer& parentContainer;

public:
	GobVirtualFile(size_t chunkOffset, size_t chunkLength, const boost::filesystem::path& filename, const GobContainer& parentContainer);

	const boost::filesystem::path& GetFilename() const;
	std::unique_ptr<IO::ReadOnlyFile> Open() const;
};

}
}
}
}
