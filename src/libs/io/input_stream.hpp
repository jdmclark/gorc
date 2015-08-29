#pragma once

#include <cstdio>
#include <type_traits>

namespace gorc {

    class output_stream;

    class input_stream {
    public:
        virtual ~input_stream();

        void read(void *dest, size_t size);
        virtual size_t read_some(void *dest, size_t size) = 0;
        virtual bool at_end() = 0;

        void copy_to(output_stream &);
    };

    template <typename T, typename StreamT>
    void read(StreamT &f, T &out)
    {
        static_assert(std::is_pod<T>::value, "cannot read non-POD type");
        f.read(&out, sizeof(T));
    }

    template <typename T, typename StreamT>
    T read(StreamT &f)
    {
        T rv;
        read(f, rv);
        return rv;
    }
}
