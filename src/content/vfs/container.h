#pragma once

#include "virtual_file.h"
#include <boost/filesystem/path.hpp>

namespace gorc {
namespace content {
namespace vfs {

class container {
public:
	virtual ~container();

	virtual size_t file_count() const = 0;
	virtual const virtual_file& get_virtual_file(size_t index) const = 0;
	virtual bool is_episode() const = 0;
	virtual const virtual_file& get_episode() const = 0;
};

}
}
}
