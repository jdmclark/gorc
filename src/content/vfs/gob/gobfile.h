#pragma once

#include "framework/io/readonlyfile.h"
#include "framework/io/nativefile.h"
#include <boost/filesystem/path.hpp>
#include <string>

namespace Gorc {
namespace Content {
namespace VFS {
namespace Gob {

class GobFile : public IO::ReadOnlyFile {
private:
	IO::NativeFile file;
	const size_t chunkOffset;
	const size_t chunkLength;
	const size_t chunkEnd;

public:
	GobFile(const boost::filesystem::path& gobPath, const boost::filesystem::path& filename, size_t chunkOffset, size_t chunkLength);

	virtual void Read(void* dest, size_t size) override;

	virtual void Seek(long offset) override;
	virtual void SetPosition(size_t offset) override;
	virtual size_t GetPosition() override;

	virtual size_t GetSize() override;
	virtual bool IsEndOfFile() override;
};

}
}
}
}
