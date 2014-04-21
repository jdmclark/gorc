#pragma once

#include "read_only_file.h"

namespace gorc {
namespace io {

class file : public read_only_file {
public:
    virtual ~file();

    virtual void write(const void* buffer, size_t size) = 0;
};

template <typename T, typename StreamT> void write(StreamT& f, const T& value) {
    static_assert(std::is_pod<T>::value, "Cannot write non-POD type");
    f.write(&value, sizeof(T));
}

}
}
