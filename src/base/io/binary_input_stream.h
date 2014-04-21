#pragma once

#include <cstdio>
#include <type_traits>
#include <iterator>

namespace gorc {
namespace io {

class binary_input_stream {
    template <typename DataT, typename StreamT>
    friend typename std::enable_if<std::is_pod<DataT>::value>::type deserialize(StreamT&, DataT&);
    template <typename DataT, typename StreamT>
    friend typename std::enable_if<std::is_pod<DataT>::value, DataT>::type deserialize(StreamT&);
private:
    virtual void read(void* src, size_t size) = 0;

public:
    virtual ~binary_input_stream();
};

template <typename T> class binary_input_stream_impl : public binary_input_stream {
    template <typename DataT, typename StreamT>
    friend typename std::enable_if<std::is_pod<DataT>::value>::type deserialize(StreamT&, DataT&);
    template <typename DataT, typename StreamT>
    friend typename std::enable_if<std::is_pod<DataT>::value, DataT>::type deserialize(StreamT&);
private:
    T& f;

    virtual void read(void* src, size_t size) override {
        f.read(src, size);
    }

public:
    binary_input_stream_impl(T& f) : f(f) {
        return;
    }
};

struct deserialization_constructor_tag { } const deserialization_constructor;

template <typename StreamT> binary_input_stream_impl<StreamT> make_binary_input_stream(StreamT& f) {
    return binary_input_stream_impl<StreamT>(f);
}

template <typename T, typename StreamT>
typename std::enable_if<std::is_pod<T>::value>::type deserialize(StreamT& f, T& v) {
    f.read(&v, sizeof(T));
}

template <typename T, typename StreamT>
typename std::enable_if<std::is_pod<T>::value, T>::type deserialize(StreamT& f) {
    T rv;
    f.read(&rv, sizeof(T));
    return rv;
}

template <typename T, typename StreamT>
typename std::enable_if<!std::is_pod<T>::value, T>::type deserialize(StreamT& f) {
    return T(deserialization_constructor, f);
}

template <typename ValueT, typename OutItT, typename StreamT>
void deserialize_range(StreamT& f, const OutItT& ins) {
    auto it = ins;
    uint32_t ct = deserialize<uint32_t>(f);
    for(int i = 0; i < ct; ++i) {
        *it = deserialize<ValueT>(f);
        ++it;
    }
}

}
}
