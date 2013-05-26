#pragma once

#include "content/vfs/container.h"
#include "framework/io/readonlyfile.h"
#include "gobvirtualfile.h"
#include <boost/filesystem/path.hpp>
#include <vector>

namespace Gorc {
namespace Content {
namespace VFS {
namespace Gob {

class GobContainer : public Container {
private:
	boost::filesystem::path gobPath;
	std::vector<GobVirtualFile> files;

public:
	GobContainer(IO::ReadOnlyFile& file);

	size_t FileCount() const;
	const VirtualFile& GetVirtualFile(size_t index) const;

	inline const boost::filesystem::path& GetPath() const {
		return gobPath;
	}
};

}
}
}
}
