#pragma once

#include <cstddef>
#include "utility/maybe.hpp"
#include "virtual_file.hpp"

namespace gorc {

    class virtual_container;

    class virtual_container_iterator {
    private:
        virtual_container const *underlying_container = nullptr;
        size_t current_index;
        maybe<virtual_file const *> current_file;

        void set_index(size_t new_index);

    public:
        virtual_container_iterator(virtual_container const &underlying_container,
                                   size_t current_index);

        virtual_file const & operator*() const;
        virtual_file const * operator->() const;

        virtual_container_iterator& operator++();
        virtual_container_iterator operator++(int);

        bool operator==(virtual_container_iterator const &it) const;
        bool operator!=(virtual_container_iterator const &it) const;
    };

}
