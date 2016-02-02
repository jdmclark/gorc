#pragma once

#include <cstddef>

namespace gorc {

    template <typename T>
    class asset_ref {
    private:
        T const *data;
        size_t id;

    public:
        asset_ref(T const &data, size_t id)
            : data(&data)
            , id(id)
        {
            return;
        }

        inline T const& operator*() const
        {
            return *data;
        }

        inline T const* operator->() const
        {
            return data;
        }
    };

}
