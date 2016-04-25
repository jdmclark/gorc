#pragma once

#include "math/size.hpp"
#include <vector>
#include <stdexcept>

namespace gorc {

    template <typename T>
    class grid {
    public:
        gorc::size<2, size_t> const size;

    private:
        std::vector<T> elements;

        inline size_t get_element_index(size_t x, size_t y) const
        {
            if(x >= gorc::get<0>(size) ||
               y >= gorc::get<1>(size)) {
                throw std::domain_error("grid::get_element_index out of bounds");
            }

            return (gorc::get<0>(size) * y) + x;
        }

    public:
        grid(gorc::size<2, size_t> const &size, T const &fill = T())
            : size(size)
            , elements(volume(size), fill)
        {
            return;
        }

        T const& get(size_t x, size_t y) const
        {
            return elements[get_element_index(x, y)];
        }

        T& get(size_t x, size_t y)
        {
            return elements[get_element_index(x, y)];
        }

        T const* data() const
        {
            return elements.data();
        }
    };

}
