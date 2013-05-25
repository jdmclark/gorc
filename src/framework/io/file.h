#pragma once

#include "readonlyfile.h"

namespace Gorc {
namespace IO {

class File : public ReadOnlyFile {
public:
	File(const std::string& filename);
	virtual ~File();

	virtual void Write(const void* buffer, size_t size) = 0;

	template <typename T> void Write(const T& value) {
		Write(&value, sizeof(T));
	}
};

}
}
