#pragma once

#include <cstdio>
#include <type_traits>
#include <iterator>

namespace gorc {
namespace io {

class binary_output_stream {
    template <typename DataT, typename StreamT>
    friend typename std::enable_if<std::is_pod<DataT>::value>::type serialize(StreamT&, const DataT&);
private:
    virtual void write(const void* src, size_t size) = 0;

public:
    virtual ~binary_output_stream();
};

template <typename T> class binary_output_stream_impl : public binary_output_stream {
    template <typename DataT, typename StreamT>
    friend typename std::enable_if<std::is_pod<DataT>::value>::type serialize(StreamT&, const DataT&);
private:
    T& f;

    virtual void write(const void* src, size_t size) override {
        f.write(src, size);
    }

public:
    binary_output_stream_impl(T& f) : f(f) {
        return;
    }
};

template <typename StreamT> binary_output_stream_impl<StreamT> make_binary_output_stream(StreamT& f) {
    return binary_output_stream_impl<StreamT>(f);
}

template <typename T, typename StreamT>
typename std::enable_if<std::is_pod<T>::value>::type serialize(StreamT& f, const T& v) {
    f.write(&v, sizeof(T));
}

template <typename T, typename StreamT>
typename std::enable_if<!std::is_pod<T>::value>::type serialize(StreamT& f, const T& v) {
    v.serialize(f);
}

template <typename RangeT, typename StreamT>
void serialize_range(StreamT& f, const RangeT& rng) {
    serialize<uint32_t>(f, static_cast<uint32_t>(std::distance(rng.begin(), rng.end())));
    for(const auto& em : rng) {
        serialize(f, em);
    }
}

}
}
