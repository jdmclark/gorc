#pragma once

#include "code_buffer.h"

namespace gorc {
namespace cog {
namespace vm {

class code_buffer_read_stream {
private:
	const code_buffer& codeBuffer;
	size_t streamPtr;

public:
	explicit code_buffer_read_stream(const code_buffer& cb);

	inline void read(void* dest, size_t size) {
		codeBuffer.read(dest, streamPtr, size);
		streamPtr += size;
	}

	template <typename T> inline T read() {
		T value = codeBuffer.read<T>(streamPtr);
		streamPtr += sizeof(T);
		return value;
	}

	inline void seek(size_t dest) {
		streamPtr = dest;
	}

	inline void scan(ptrdiff_t offset) {
		streamPtr += offset;
	}

	inline size_t tell() const {
		return streamPtr;
	}
};

}
}
}
