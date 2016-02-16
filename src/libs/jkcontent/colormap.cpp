#include "colormap.hpp"
#include "log/log.hpp"
#include "utility/range.hpp"
#include <cstring>

gorc::fourcc const gorc::colormap::type = "CMP"_4CC;

gorc::colormap_palette::colormap_palette(deserialization_constructor_tag,
                                         binary_input_stream &bis)
{
    bis.read(data.data(), data.size() * sizeof(color_rgb8));
}

void gorc::colormap_palette::json_serialize_object(json_output_stream &jos) const
{
    json_serialize_array(jos, data);
}

gorc::colormap_light_level::colormap_light_level(deserialization_constructor_tag,
                                                 binary_input_stream &bis)
{
    bis.read(data.data(), data.size() * sizeof(uint8_t));
}

void gorc::colormap_light_level::json_serialize_object(json_output_stream &jos) const
{
    json_serialize_array(jos, data);
}

gorc::colormap_transparency_table::colormap_transparency_table(deserialization_constructor_tag,
                                                               binary_input_stream &bis)
{
    bis.read(data.data(), data.size() * sizeof(uint8_t));
}

void gorc::colormap_transparency_table::json_serialize_object(json_output_stream &jos) const
{
    json_serialize_array(jos, data);
}

gorc::colormap::colormap(deserialization_constructor_tag, binary_input_stream &bis)
{
    bis.read(magic.data(), sizeof(char) * 4);

    if(strncmp(magic.data(), "CMP ", 4) != 0) {
        LOG_FATAL("file is not a valid colormap");
    }

    version = binary_deserialize<uint32_t>(bis);

    if(version != 0x1E) {
        LOG_FATAL(format("colormap has unknown version %d") % version);
    }

    transparency = binary_deserialize<uint32_t>(bis);

    tint = binary_deserialize<color_rgb>(bis);

    // Read padding/unknowns
    for(size_t i = 0; i < 10; ++i) {
        unknown[i] = binary_deserialize<uint32_t>(bis);
    }

    // Read palette table. 24-bit RGB.
    palette = std::make_unique<colormap_palette>(deserialization_constructor, bis);

    // Read light levels
    for(size_t i = 0; i < 64; ++i) {
        light_levels.emplace_back(deserialization_constructor, bis);
    }

    if(transparency != 0) {
        for(size_t i = 0; i < 256; ++i) {
            transparency_tables.emplace_back(deserialization_constructor, bis);
        }
    }
}

void gorc::colormap::json_serialize_object(json_output_stream &jos) const
{
    json_serialize_members(jos, [&]{
        json_serialize_member(jos, "magic", [&]{
                json_serialize(jos, std::string(magic.data(), 4));
            });

        json_serialize_member(jos, "version", [&]{ json_serialize(jos, version); });
        json_serialize_member(jos, "has_transparency", [&] { json_serialize(jos, transparency); });
        json_serialize_member(jos, "tint", [&]{ json_serialize(jos, tint); });
        json_serialize_member(jos, "unknown", [&]{ json_serialize_array(jos, unknown); });

        json_serialize_member(jos, "palette", [&]{ palette->json_serialize_object(jos); });
        json_serialize_member(jos, "light_levels", [&]{ json_serialize_array(jos, light_levels); });

        if(!transparency_tables.empty()) {
            json_serialize_member(jos, "transparency_tables", [&]{
                    json_serialize_array(jos, transparency_tables);
                });
        }
    });
}
