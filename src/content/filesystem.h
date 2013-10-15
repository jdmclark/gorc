#pragma once

#include "framework/io/read_only_file.h"
#include <memory>
#include <boost/filesystem/path.hpp>

namespace gorc {
namespace content {

class filesystem {
public:
	virtual ~filesystem();
	virtual std::unique_ptr<io::read_only_file> open(const boost::filesystem::path& path) const = 0;
};

}
}
