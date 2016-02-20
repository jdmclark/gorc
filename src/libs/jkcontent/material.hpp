#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <memory>

#include "content/asset.hpp"
#include "math/color.hpp"
#include "io/binary_input_stream.hpp"
#include "text/json_output_stream.hpp"

namespace gorc {

    class material_cel_record {
    public:
        uint32_t type;
        uint32_t color_index;
        std::array<uint32_t, 4> unknown_color;
        std::array<uint32_t, 3> unknown_texture;
        uint32_t texture_index;

        material_cel_record(deserialization_constructor_tag, binary_input_stream &);
        void json_serialize_object(json_output_stream &) const;
    };

    class material_texture_record {
    public:
        uint32_t width;
        uint32_t height;
        std::array<uint32_t, 3> unknown;
        uint32_t mipmap_count;
        std::vector<std::vector<uint8_t>> image_data;

        material_texture_record(deserialization_constructor_tag,
                                binary_input_stream &);
        void json_serialize_object(json_output_stream &,
                                   std::string const &base_texture_name) const;
    };

    class material {
    public:
        static fourcc const type;

        std::array<char, 4> magic;
        uint32_t version;

        uint32_t material_type;
        uint32_t record_count;
        uint32_t texture_count;

        uint32_t transparency;
        uint32_t bitdepth;

        std::array<uint32_t, 12> unknown;

        std::vector<material_cel_record> cel_records;
        std::vector<material_texture_record> texture_records;

        material(deserialization_constructor_tag, binary_input_stream &);

        void json_serialize_object(json_output_stream &) const;
    };

}
