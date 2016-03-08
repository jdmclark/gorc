#pragma once

#include "code_buffer.hpp"

namespace gorc {
namespace cog {
namespace vm {

class code_buffer_write_stream {
private:
    code_buffer& codeBuffer;
    size_t streamPtr;

public:
    explicit code_buffer_write_stream(code_buffer& cb);

    inline void write(const void* src, size_t size) {
        codeBuffer.write(streamPtr, src, size);
        streamPtr += size;
    }

    template <typename T> inline void write(T value) {
        codeBuffer.write<T>(value, streamPtr);
        streamPtr += sizeof(T);
    }

    inline size_t write_buffer(code_buffer& cb) {
        size_t retVal = streamPtr;
        size_t size = cb.size();

        codeBuffer.write(streamPtr, cb.read_pointer(0, size), size);
        streamPtr += size;

        return retVal;
    }

    inline void seek(size_t dest) {
        streamPtr = dest;
    }

    inline size_t tell() const {
        return streamPtr;
    }
};

}
}
}
