#pragma once

#include <cstddef>
#include "io/binary_input_stream.hpp"
#include "io/binary_output_stream.hpp"

namespace gorc {

    class asset;
    asset const& get_asset_from_id(service_registry const &, size_t id);

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

        asset_ref(deserialization_constructor_tag, binary_input_stream &bis)
            : data(nullptr)
            , id(binary_deserialize<size_t>(bis))
        {
            // Initialize data pointer
            data = &dynamic_cast<T const &>(get_asset_from_id(bis.services, id));
        }

        void binary_serialize_object(binary_output_stream &bos) const
        {
            binary_serialize(bos, id);
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
