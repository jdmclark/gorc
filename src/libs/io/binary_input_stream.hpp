#pragma once

#include <type_traits>
#include <string>
#include "input_stream.hpp"

namespace gorc {

    class binary_input_stream {
    private:
        input_stream &stream;

    public:
        explicit binary_input_stream(input_stream &stream);

        template <typename T>
        typename std::enable_if<std::is_fundamental<T>::value, void>::type read_value(T &out)
        {
            stream.read(&out, sizeof(T));
        }

        std::string read_string();

    };

    struct binary_deserialization_constructor {
        binary_deserialization_constructor();
    };

    binary_deserialization_constructor const binary_deserialization_constructor_tag;

    template <typename T>
    typename std::enable_if<std::is_fundamental<T>::value, T>::type
        binary_deserialize(binary_input_stream &is)
    {
        T rv;
        is.read_value(rv);
        return rv;
    }

    template <typename T>
    typename std::enable_if<std::is_same<std::string, T>::value, T>::type
        binary_deserialize(binary_input_stream &is)
    {
        return is.read_string();
    }

    template <typename T>
    typename std::enable_if<!std::is_same<std::string, T>::value &&
                            !std::is_fundamental<T>::value, T>::type
        binary_deserialize(binary_input_stream &is)
    {
        return T(binary_deserialization_constructor_tag, is);
    }

    template <typename BackInsertIt, typename FmtFnT>
    void binary_deserialize_range(binary_input_stream &is,
                                  BackInsertIt it,
                                  FmtFnT fn)
    {
        size_t amt = binary_deserialize<size_t>(is);
        for(size_t i = 0; i < amt; ++i, ++it) {
            *it = fn(is);
        }
    }

    template <typename T, typename BackInsertIt>
    void binary_deserialize_range(binary_input_stream &is, BackInsertIt it)
    {
        size_t amt = binary_deserialize<size_t>(is);
        for(size_t i = 0; i < amt; ++i, ++it) {
            *it = binary_deserialize<T>(is);
        }
    }

}
