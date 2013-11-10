#pragma once

#include "content/vfs/container.h"
#include "framework/io/read_only_file.h"
#include "gob_virtual_file.h"
#include <boost/filesystem/path.hpp>
#include <vector>

namespace gorc {
namespace content {
namespace vfs {
namespace gob {

class gob_container : public container {
private:
	boost::filesystem::path gobPath;
	std::vector<gob_virtual_file> files;
	bool isEpisode;
	size_t episodeIndex;

public:
	gob_container(io::read_only_file& file);

	size_t file_count() const;
	const virtual_file& get_virtual_file(size_t index) const;
	bool is_episode() const;
	const virtual_file& get_episode() const;

	inline const boost::filesystem::path& get_path() const {
		return gobPath;
	}
};

}
}
}
}
