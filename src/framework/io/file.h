#pragma once

#include "read_only_file.h"

namespace Gorc {
namespace IO {

class File : public ReadOnlyFile {
public:
	File(const boost::filesystem::path& filename);
	virtual ~File();

	virtual void Write(const void* buffer, size_t size) = 0;

	template <typename T> void Write(const T& value) {
		Write(&value, sizeof(T));
	}
};

}
}
