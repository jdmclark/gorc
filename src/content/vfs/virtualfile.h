#pragma once

#include "framework/io/readonlyfile.h"
#include <boost/filesystem/path.hpp>
#include <memory>

namespace Gorc {
namespace Content {
namespace VFS {

class VirtualFile {
public:
	virtual ~VirtualFile();

	virtual const boost::filesystem::path& GetFilename() const = 0;
	virtual std::unique_ptr<IO::ReadOnlyFile> Open() const = 0;
};

}
}
}
