#include "material.hpp"
#include "log/log.hpp"
#include "utility/range.hpp"
#include "io/path.hpp"
#include "content/image.hpp"
#include "text/extract_path.hpp"
#include <cstring>

gorc::fourcc const gorc::material::type = "MAT"_4CC;

gorc::material_cel_record::material_cel_record(deserialization_constructor_tag,
                                               binary_input_stream &bis)
{
    type = binary_deserialize<uint32_t>(bis);
    if(type != 0 && type != 8) {
        LOG_FATAL(format("unknown cel type %d") % type);
    }

    color_index = binary_deserialize<uint32_t>(bis);
    for(auto &em : unknown_color) {
        em = binary_deserialize<uint32_t>(bis);
    }

    if(type == 8) {
        // Texture type: load texture elements
        for(auto &em : unknown_texture) {
            em = binary_deserialize<uint32_t>(bis);
        }

        texture_index = binary_deserialize<uint32_t>(bis);
    }
}

void gorc::material_cel_record::json_serialize_object(json_output_stream &jos) const
{
    json_serialize_members(jos, [&]{
        json_serialize_member(jos, "type", [&]{ json_serialize(jos, type); });
        json_serialize_member(jos, "color", [&]{ json_serialize(jos, color_index); });
        json_serialize_member(jos, "unknown_color", [&]{
                json_serialize_array(jos, unknown_color);
            });

        if(type == 8) {
            // Texture type: serialize texture elements
            json_serialize_member(jos, "unknown_texture", [&]{
                    json_serialize_array(jos, unknown_texture);
                });
            json_serialize_member(jos, "texture_index", [&]{ json_serialize(jos, texture_index); });
        }
    });
}

gorc::material_texture_record::material_texture_record(deserialization_constructor_tag,
                                                       binary_input_stream &bis)
{
    width = binary_deserialize<uint32_t>(bis);
    height = binary_deserialize<uint32_t>(bis);

    for(auto &em : unknown) {
        em = binary_deserialize<uint32_t>(bis);
    }

    mipmap_count = binary_deserialize<uint32_t>(bis);

    uint32_t next_width = width;
    uint32_t next_height = height;
    for(uint32_t i = 0; i < mipmap_count; ++i) {
        image_data.emplace_back();
        auto &data = image_data.back();

        data.resize(next_width * next_height);
        bis.read(&data[0], data.size());

        next_width >>= 1;
        next_height >>= 1;
    }
}

void gorc::material_texture_record::json_serialize_object(json_output_stream &jos,
                                                          std::string const &base_texture_name) const
{
    json_serialize_members(jos, [&]{
        json_serialize_member(jos, "width", [&]{ json_serialize(jos, width); });
        json_serialize_member(jos, "height", [&]{ json_serialize(jos, height); });
        json_serialize_member(jos, "unknown", [&]{ json_serialize_array(jos, unknown); });
        json_serialize_member(jos, "mipmap_count", [&]{ json_serialize(jos, mipmap_count); });

        json_serialize_member(jos, "mipmaps", [&]{
                uint32_t texture_num = 0;
                uint32_t next_width = width;
                uint32_t next_height = height;
                json_serialize_array(jos, image_data, [&](auto const &data) {
                        image img(make_size(next_width, next_height));
                        size_t idx = 0;
                        for(size_t y = 0; y < next_height; ++y) {
                            for(size_t x = 0; x < next_width && idx < data.size(); ++x, ++idx) {
                                img.get(x, y) = make_color_rgba8(data[idx],
                                                                 data[idx],
                                                                 data[idx],
                                                                 255);
                            }
                        }

                        std::string map_name = str(format("%s-%d.png") %
                                                   base_texture_name %
                                                   texture_num);

                        save_image_to_file(img, map_name);
                        json_serialize(jos, map_name);

                        ++texture_num;
                        next_width >>= 1;
                        next_height >>= 1;
                    });
            });
    });
}

gorc::material::material(deserialization_constructor_tag, binary_input_stream &bis)
{
    bis.read(magic.data(), sizeof(char) * 4);

    if(strncmp(magic.data(), "MAT ", 4) != 0) {
        LOG_FATAL("file is not a valid material");
    }

    version = binary_deserialize<uint32_t>(bis);

    if(version != 0x32) {
        LOG_FATAL(format("material has unknown version %d") % version);
    }

    material_type = binary_deserialize<uint32_t>(bis);
    record_count = binary_deserialize<uint32_t>(bis);
    texture_count = binary_deserialize<uint32_t>(bis);

    transparency = binary_deserialize<uint32_t>(bis);
    bitdepth = binary_deserialize<uint32_t>(bis);

    if(bitdepth != 8) {
        LOG_FATAL(format("bitdepth %d is not supported") % bitdepth);
    }

    for(auto &em : unknown) {
        em = binary_deserialize<uint32_t>(bis);
    }

    for(uint32_t i = 0; i < record_count; ++i) {
        cel_records.emplace_back(deserialization_constructor, bis);
    }

    for(uint32_t i = 0; i < texture_count; ++i) {
        texture_records.emplace_back(deserialization_constructor, bis);
    }
}

void gorc::material::json_serialize_object(json_output_stream &jos) const
{
    path extract_path = jos.services.get<gorc::extract_path>().value;

    json_serialize_members(jos, [&]{
        json_serialize_member(jos, "magic", [&]{
                json_serialize(jos, std::string(magic.data(), 4));
            });

        json_serialize_member(jos, "version", [&]{ json_serialize(jos, version); });

        json_serialize_member(jos, "type", [&]{ json_serialize(jos, material_type); });
        json_serialize_member(jos, "record_count", [&]{ json_serialize(jos, record_count); });
        json_serialize_member(jos, "texture_count", [&]{ json_serialize(jos, texture_count); });
        json_serialize_member(jos, "transparency", [&]{ json_serialize(jos, transparency); });
        json_serialize_member(jos, "bitdepth", [&]{ json_serialize(jos, bitdepth); });

        json_serialize_member(jos, "unknown", [&]{ json_serialize_array(jos, unknown); });

        json_serialize_member(jos, "cels", [&]{ json_serialize_array(jos, cel_records); });

        json_serialize_member(jos, "textures", [&]{
                int tex_num = 0;
                json_serialize_array(jos, texture_records, [&](auto const &em) {
                        std::string texture_base_name = str(format("%s-%d") %
                                                            extract_path.generic_string() %
                                                            tex_num);
                        em.json_serialize_object(jos, texture_base_name);
                        ++tex_num;
                    });
            });
    });
}
