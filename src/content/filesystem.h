#pragma once

#include "framework/io/readonlyfile.h"
#include <memory>
#include <boost/filesystem/path.hpp>

namespace Gorc {
namespace Content {

class FileSystem {
public:
	virtual ~FileSystem();
	virtual std::unique_ptr<IO::ReadOnlyFile> Open(const boost::filesystem::path& path) const = 0;
};

}
}
