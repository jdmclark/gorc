#pragma once

#include "filesystem.h"

namespace Gorc {
namespace Content {

class NativeFileSystem : public FileSystem {
private:
	const boost::filesystem::path basepath;
public:
	NativeFileSystem(const boost::filesystem::path& basepath);

	virtual std::unique_ptr<Gorc::IO::ReadOnlyFile> Open(const boost::filesystem::path& name) const override;
};

}
}
