#pragma once

#include "cog/script/value.hpp"
#include <vector>

namespace gorc {
    namespace cog {

        class heap {
        private:
            std::vector<value> values;

        public:
            heap() = default;
            heap(size_t initial_size);

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
