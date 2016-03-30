#pragma once

#include <cstddef>
#include "id.hpp"
#include "io/binary_input_stream.hpp"
#include "io/binary_output_stream.hpp"

namespace gorc {

    MAKE_ID_TYPE(asset);

    class asset;
    asset const& get_asset_from_id(service_registry const &, asset_id id);

    template <typename T>
    class asset_ref {
    private:
        T const *data;
        asset_id id;

    public:
        asset_ref(T const &data, asset_id id)
            : data(&data)
            , id(id)
        {
            return;
        }

        asset_ref(deserialization_constructor_tag, binary_input_stream &bis)
            : data(nullptr)
            , id(binary_deserialize<asset_id>(bis))
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

        inline bool operator==(asset_ref<T> const &ref) const
        {
            return id == ref.id;
        }

        inline bool operator!=(asset_ref<T> const &ref) const
        {
            return id != ref.id;
        }

        inline asset_id get_id() const
        {
            return id;
        }
    };

}
