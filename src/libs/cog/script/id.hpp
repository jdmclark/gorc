#pragma once

#include <cstdint>

#define MAKE_ID(x) \
    class x##_tag { }; \
    using x##_id = id<x##_tag>

namespace gorc {
    namespace cog {

        class invalid_type { };
        constexpr invalid_type invalid_id = invalid_type();

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

            id(invalid_type const &)
                : value(-1)
            {
                return;
            }

            explicit id(int32_t value)
                : value(value)
            {
                return;
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

            bool operator==(id const &o)
            {
                return value == o.value;
            }

            bool operator!=(id const &o)
            {
                return value != o.value;
            }
        };

        template <typename TagT>
        bool operator==(invalid_type const &a, id<TagT> const &b)
        {
            return id<TagT>(a) == b;
        }

        template <typename TagT>
        bool operator!=(invalid_type const &a, id<TagT> const &b)
        {
            return id<TagT>(a) != b;
        }

        // Entity IDs
        MAKE_ID(sector);
        MAKE_ID(surface);
        MAKE_ID(thing);
        MAKE_ID(player);

        // Asset IDs
        MAKE_ID(ai);
        MAKE_ID(cog);
        MAKE_ID(keyframe);
        MAKE_ID(material);
        MAKE_ID(model);
        MAKE_ID(sound);
        MAKE_ID(thing_template);

        // Cog subsystem IDs
        MAKE_ID(verb);
        MAKE_ID(message);
    }
}

#undef MAKE_ID
