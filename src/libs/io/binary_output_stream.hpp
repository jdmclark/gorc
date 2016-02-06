#pragma once

#include <type_traits>
#include <string>
#include "output_stream.hpp"
#include "utility/service_registry.hpp"
#include "utility/time.hpp"

namespace gorc {

    class binary_output_stream : public output_stream {
    private:
        output_stream &stream;

    public:
        service_registry const &services;

        explicit binary_output_stream(output_stream &stream);
        binary_output_stream(output_stream &stream, service_registry const &services);

        template <typename T>
        typename std::enable_if<std::is_fundamental<T>::value, void>::type write_value(T &out)
        {
            stream.write(&out, sizeof(T));
        }

        void write_string(std::string const &str);

        virtual size_t write_some(void const *src, size_t size) override;
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
    auto binary_serialize(binary_output_stream &os, T const &value)
        -> typename std::conditional<true, void, decltype(value.binary_serialize_object(os))>::type
    {
        value.binary_serialize_object(os);
    }

    template <typename T>
    typename std::enable_if<std::is_enum<T>::value, void>::type
        binary_serialize(binary_output_stream &os, T const &value)
    {
        binary_serialize(os, static_cast<typename std::underlying_type<T>::type>(value));
    }

    template <typename T>
    typename std::enable_if<std::is_same<time_delta, T>::value, void>::type
        binary_serialize(binary_output_stream &os, T const &value)
    {
        binary_serialize<double>(os, value.count());
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
