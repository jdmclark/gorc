#pragma once

#include "cog/script/value.hpp"
#include "io/binary_input_stream.hpp"
#include "io/binary_output_stream.hpp"
#include <vector>

namespace gorc {
    namespace cog {

        class heap {
        private:
            std::vector<value> values;

        public:
            heap() = default;
            explicit heap(size_t initial_size);
            heap(deserialization_constructor_tag, binary_input_stream &);

            void binary_serialize_object(binary_output_stream &) const;

            value& operator[](size_t index);
            value operator[](size_t index) const;

            size_t size() const;

            inline auto begin() -> decltype(values.begin())
            {
                return values.begin();
            }

            inline auto end() -> decltype(values.end())
            {
                return values.end();
            }
        };

    }
}
