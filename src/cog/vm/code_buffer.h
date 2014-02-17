#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>

namespace gorc {
namespace cog {
namespace vm {

class code_buffer
{
private:
    std::vector<uint8_t> data;
    size_t topPtr;

    void throwCodeBufferOverflowException() const;

    inline void resizeDataForWrite(size_t dest, size_t size) {
        if((dest + size) > topPtr) {
            topPtr = dest + size;
        }

        if(topPtr > data.size()) {
            data.resize(topPtr * 2, 0);
        }
    }

    inline void tryRead(size_t src, size_t size) const {
        if((src + size) > topPtr) {
            throwCodeBufferOverflowException();
        }
    }

public:
    code_buffer();

    void write(size_t dest, const void* src, size_t size);
    void read(void* dest, size_t src, size_t size) const;

    template <typename T> inline void write(T value, size_t dest)
    {
        resizeDataForWrite(dest, sizeof(T));
        *(T*)(&data[dest]) = value;
    }

    template <typename T> inline T read(size_t src) const
    {
        tryRead(src, sizeof(T));
        return *(T*)(&data[src]);
    }

    inline void* write_pointer(size_t dest, size_t size)
    {
        resizeDataForWrite(dest, size);
        return (void*)&data[dest];
    }

    inline const void* read_pointer(size_t src, size_t size) const
    {
        tryRead(src, size);
        return (const void*)&data[src];
    }

    inline size_t size() const
    {
        return topPtr;
    }
};
}
}
}
