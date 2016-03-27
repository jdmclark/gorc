#pragma once

#include <type_traits>
#include <string>
#include "input_stream.hpp"
#include "utility/constructor_tag.hpp"
#include "utility/service_registry.hpp"
#include "utility/time.hpp"
#include "utility/flag_set.hpp"

namespace gorc {

    class binary_input_stream : public input_stream {
    private:
        input_stream &stream;

    public:
        service_registry const &services;

        explicit binary_input_stream(input_stream &stream);
        binary_input_stream(input_stream &stream, service_registry const &services);

        template <typename T>
        typename std::enable_if<std::is_fundamental<T>::value, void>::type read_value(T &out)
        {
            stream.read(&out, sizeof(T));
        }

        std::string read_string();

        virtual size_t read_some(void *dest, size_t size) override;
        virtual bool at_end() override;
    };

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
    typename std::enable_if<std::is_same<time_delta, T>::value, T>::type
        binary_deserialize(binary_input_stream &is)
    {
        return time_delta(binary_deserialize<double>(is));
    }

    template <typename T>
    typename std::enable_if<std::is_enum<T>::value, T>::type
        binary_deserialize(binary_input_stream &is)
    {
        return static_cast<T>(binary_deserialize<typename std::underlying_type<T>::type>(is));
    }

    template <typename T>
    typename std::enable_if<is_flag_set<T>::value, T>::type
        binary_deserialize(binary_input_stream &is)
    {
        return T(binary_deserialize<typename flag_set_underlying_type<T>::type>(is));
    }

    template <typename T>
    auto binary_deserialize(binary_input_stream &is)
        -> decltype(T(deserialization_constructor, is))
    {
        return T(deserialization_constructor, is);
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
