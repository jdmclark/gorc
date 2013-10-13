#pragma once

#include "content/vfs/container.h"
#include "framework/io/read_only_file.h"
#include "gob_virtual_file.h"
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
	bool isEpisode;
	size_t episodeIndex;

public:
	GobContainer(IO::ReadOnlyFile& file);

	size_t FileCount() const;
	const VirtualFile& GetVirtualFile(size_t index) const;
	bool IsEpisode() const;
	const VirtualFile& GetEpisode() const;

	inline const boost::filesystem::path& GetPath() const {
		return gobPath;
	}
};

}
}
}
}
