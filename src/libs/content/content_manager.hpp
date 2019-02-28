#pragma once

#include "asset.hpp"
#include "asset_ref.hpp"
#include "fourcc.hpp"
#include "io/binary_input_stream.hpp"
#include "io/binary_output_stream.hpp"
#include "log/diagnostic_context.hpp"
#include "utility/maybe.hpp"
#include "utility/service_registry.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace gorc {

    class content_manager {
    private:
        class asset_data {
        public:
            fourcc type;
            std::string name;
            std::unique_ptr<asset> content;

            asset_data(fourcc type, std::string const &name);
            asset_data(deserialization_constructor_tag, binary_input_stream &);

            void binary_serialize_object(binary_output_stream &) const;
        };

        service_registry const &services;
        std::vector<asset_data> assets;
        std::unordered_map<std::string, asset_id> asset_map;

        asset_id load_internal(fourcc type, std::string const &name);
        void finalize_internal(asset_id id);

    public:
        explicit content_manager(service_registry const &services);
        content_manager(deserialization_constructor_tag, binary_input_stream &);

        void binary_serialize_object(binary_output_stream &) const;

        template <typename T>
        asset_ref<T> load(std::string const &name)
        {
            diagnostic_context dc(name.c_str());
            asset_id ref_id = load_internal(T::type, name);
            auto const &record = at_id(assets, ref_id);
            return asset_ref<T>(dynamic_cast<T const &>(*record.content), ref_id);
        }

        asset const &load_from_id(asset_id id);

        template <typename T>
        asset_id load_id(std::string const &name)
        {
            diagnostic_context dc(name.c_str());
            return load_internal(T::type, name);
        }

        template <typename IdT>
        asset_ref<typename id_asset_type<IdT>::type> get_asset(IdT id)
        {
            using T = typename id_asset_type<IdT>::type;
            asset_id ut_id(static_cast<int>(id));
            return asset_ref<T>(dynamic_cast<T const &>(load_from_id(ut_id)), ut_id);
        }
    };

    template <typename IdT>
    auto get_asset(content_manager &cm, IdT id)
    {
        return cm.get_asset(id);
    }
}
