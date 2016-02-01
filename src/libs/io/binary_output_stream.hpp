#pragma once

#include <type_traits>
#include <string>
#include "output_stream.hpp"

namespace gorc {

    class binary_output_stream {
    private:
        output_stream &stream;

    public:
        explicit binary_output_stream(output_stream &stream);

        template <typename T>
        typename std::enable_if<std::is_fundamental<T>::value, void>::type write_value(T &out)
        {
            stream.write(&out, sizeof(T));
        }

        void write_string(std::string const &str);
    };

    template <typename T>
    typename std::enable_if<std::is_fundamental<T>::value, void>::type
        binary_serialize(binary_output_stream &os, T const &value)
    {
        os.write_value(value);
    }

    template <typename T>
    typename std::enable_if<std::is_same<std::string, T>::value, void>::type
        binary_serialize(binary_output_stream &os, T const &value)
    {
        os.write_string(value);
    }

    template <typename T>
    typename std::enable_if<!std::is_same<std::string, T>::value &&
                            !std::is_fundamental<T>::value, void>::type
        binary_serialize(binary_output_stream &os, T const &value)
    {
        value.binary_serialize_object(os);
    }

    template <typename RangeT, typename FmtFnT>
    void binary_serialize_range(binary_output_stream &os,
                                RangeT const &rng,
                                FmtFnT fn)
    {
        binary_serialize<size_t>(os, rng.size());
        for(auto const &em : rng) {
            fn(os, em);
        }
    }

    template <typename RangeT>
    void binary_serialize_range(binary_output_stream &os, RangeT const &rng)
    {
        binary_serialize<size_t>(os, rng.size());
        for(auto const &em : rng) {
            binary_serialize(os, em);
        }
    }

}
