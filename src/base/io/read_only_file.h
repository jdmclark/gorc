#pragma once

#include <string>
#include <type_traits>
#include <vector>
#include <unordered_map>

namespace gorc {
namespace io {

class read_only_file {
public:
    virtual ~read_only_file();

    virtual void read(void* dest, size_t size) = 0;

    virtual void seek(int offset) = 0;
    virtual void set_position(size_t offset) = 0;
    virtual size_t position() = 0;

    virtual size_t size() = 0;
    virtual bool at_end() = 0;
};

template <typename T, typename StreamT> T read(StreamT& f) {
    static_assert(std::is_pod<T>::value, "Cannot read non-POD type");
    T rv;
    f.read(&rv, sizeof(T));
    return rv;
}

}
}
