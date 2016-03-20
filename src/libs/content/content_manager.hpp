#pragma once

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "utility/maybe.hpp"
#include "asset.hpp"
#include "fourcc.hpp"
#include "utility/service_registry.hpp"
#include "asset_ref.hpp"
#include "log/diagnostic_context.hpp"
#include "io/binary_output_stream.hpp"
#include "io/binary_input_stream.hpp"

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

        asset const& load_from_id(asset_id id);

        template <typename T>
        asset_id load_id(std::string const &name)
        {
            diagnostic_context dc(name.c_str());
            return load_internal(T::type, name);
        }

        template <typename T>
        asset_ref<T> get_asset(asset_id id)
        {
            return asset_ref<T>(dynamic_cast<T const &>(load_from_id(id)), id);
        }
    };

}
