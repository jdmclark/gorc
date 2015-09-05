#pragma once

#include "virtual_file.hpp"
#include "virtual_container_iterator.hpp"

namespace gorc {

    class virtual_container {
        friend class virtual_container_iterator;

    protected:
        virtual virtual_file const& get_file(size_t index) const = 0;

    public:
        virtual ~virtual_container();

        virtual size_t size() const = 0;

        virtual_container_iterator begin() const;
        virtual_container_iterator end() const;
    };

}
