#pragma once

#include <cstdio>
#include <boost/filesystem/path.hpp>
#include "file.h"

namespace Gorc {
namespace IO {

class NativeFile : public File {
private:
	FILE* hFile;

	bool open(const boost::filesystem::path& filename);
	bool create(const boost::filesystem::path& filename);
	bool close();

public:
	NativeFile(const boost::filesystem::path& filename, bool createNew = false);
	~NativeFile();

	void Read(void* dest, size_t length);
	void Write(const void* buffer, size_t length);

	void Seek(long offset);
	void SetPosition(size_t offset);
	size_t GetPosition();

	size_t GetSize();
	bool IsEndOfFile();
};

}
}
