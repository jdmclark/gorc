#pragma once

#include <cstdint>
#include "io/binary_input_stream.hpp"
#include "io/binary_output_stream.hpp"
#include "log/log.hpp"

#define MAKE_ID_TYPE(x) \
    class x##_tag { }; \
    using x##_id = id<x##_tag>

namespace gorc {

    class invalid_id_type { };
    constexpr invalid_id_type invalid_id = invalid_id_type();

    template <typename TagT>
    class id {
    private:
        int32_t value;

    public:
        id()
            : value(-1)
        {
            return;
        }

        id(invalid_id_type const &)
            : value(-1)
        {
            return;
        }

        explicit id(int32_t value)
            : value(value)
        {
            return;
        }

        explicit id(size_t value)
            : value(static_cast<int32_t>(value))
        {
            return;
        }

        id(deserialization_constructor_tag, binary_input_stream &bis)
            : value(binary_deserialize<int32_t>(bis))
        {
            return;
        }

        void binary_serialize_object(binary_output_stream &bos) const
        {
            binary_serialize<int32_t>(bos, value);
        }

        explicit operator int32_t() const
        {
            return value;
        }

        id& operator=(id const &o)
        {
            value = o.value;
            return *this;
        }

        bool operator==(id const &o) const
        {
            return value == o.value;
        }

        bool operator!=(id const &o) const
        {
            return value != o.value;
        }

        bool is_valid() const
        {
            return value != -1;
        }
    };

    template <typename TagT>
    bool operator==(invalid_id_type const &a, id<TagT> const &b)
    {
        return id<TagT>(a) == b;
    }

    template <typename TagT>
    bool operator!=(invalid_id_type const &a, id<TagT> const &b)
    {
        return id<TagT>(a) != b;
    }

    template <typename RangeT, typename IdT>
    size_t internal_id_to_index(char const *file, int line, RangeT const &rng, IdT id)
    {
        int32_t value = static_cast<int32_t>(id);
        if(value < 0 || value >= static_cast<int32_t>(rng.size())) {
            ::gorc::write_log_message(file,
                                      line,
                                      log_level::error,
                                      str(format("id %d is out of bounds") % value));
            throw ::gorc::logged_runtime_error();
        }

        return static_cast<size_t>(value);
    }

    template <typename RangeT, typename IdT>
    auto const& internal_at_id(char const *file, int line, RangeT const &rng, IdT id)
    {
        return rng[internal_id_to_index(file, line, rng, id)];
    }

    template <typename RangeT, typename IdT>
    auto & internal_at_id(char const *file, int line, RangeT &rng, IdT id)
    {
        return rng[internal_id_to_index(file, line, rng, id)];
    }

#define at_id(x, y) \
    ::gorc::internal_at_id(__FILE__, __LINE__, x, y)

    // Entity IDs
    MAKE_ID_TYPE(sector);
    MAKE_ID_TYPE(surface);
    MAKE_ID_TYPE(thing);
    MAKE_ID_TYPE(player);

    // Asset IDs
    MAKE_ID_TYPE(ai);
    MAKE_ID_TYPE(cog);
    MAKE_ID_TYPE(colormap);
    MAKE_ID_TYPE(keyframe);
    MAKE_ID_TYPE(material);
    MAKE_ID_TYPE(model);
    MAKE_ID_TYPE(sound);
    MAKE_ID_TYPE(thing_template);

    // Cog subsystem IDs
    MAKE_ID_TYPE(verb);
    MAKE_ID_TYPE(message);
}

namespace std {

    template <typename TagT>
    struct underlying_type<::gorc::id<TagT>> {
        using type = int32_t;
    };

}
