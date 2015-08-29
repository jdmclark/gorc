#pragma once

#include <cstdio>
#include <type_traits>

namespace gorc {

    class output_stream {
    public:
        virtual ~output_stream();

        void write(void const *src, size_t size);
        virtual size_t write_some(void const *src, size_t size) = 0;
    };

    template <typename T, typename StreamT>
    void write(StreamT &f, T const &value)
    {
        static_assert(std::is_pod<T>::value, "cannot write non-POD type");
        f.write(&value, sizeof(T));
    }

}
