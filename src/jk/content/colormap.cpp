#include "colormap.hpp"
#include "log/log.hpp"
#include "utility/range.hpp"
#include "io/path.hpp"
#include "content/image.hpp"
#include "text/extract_path.hpp"
#include "content/content_manager.hpp"
#include <cstring>

gorc::fourcc const gorc::colormap::type = "CMP"_4CC;

gorc::colormap_palette::colormap_palette(uninit_constructor_tag)
{
    return;
}

gorc::colormap_palette::colormap_palette(deserialization_constructor_tag,
                                         binary_input_stream &bis)
{
    bis.read(data.data(), data.size() * sizeof(color_rgb8));
}

gorc::colormap_light_level::colormap_light_level(uninit_constructor_tag)
{
    return;
}

gorc::colormap_light_level::colormap_light_level(deserialization_constructor_tag,
                                                 binary_input_stream &bis)
{
    for(auto &row : data) {
        bis.read(row.data(), row.size() * sizeof(uint8_t));
    }
}

gorc::colormap_transparency_table::colormap_transparency_table(deserialization_constructor_tag,
                                                               binary_input_stream &bis)
{
    for(auto &row : data) {
        bis.read(row.data(), row.size() * sizeof(uint8_t));
    }
}

gorc::colormap::colormap(uninit_constructor_tag)
{
    return;
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

    transparency = (binary_deserialize<uint32_t>(bis) != 0);

    tint = binary_deserialize<color_rgb>(bis);

    // Read padding/unknowns
    for(size_t i = 0; i < 10; ++i) {
        unknown[i] = binary_deserialize<uint32_t>(bis);
    }

    // Read palette table. 24-bit RGB.
    palette = std::make_unique<colormap_palette>(deserialization_constructor, bis);

    // Read light levels
    light_levels = std::make_unique<colormap_light_level>(deserialization_constructor, bis);

    if(transparency != 0) {
        transparency_tables = std::make_unique<colormap_transparency_table>(
                                                deserialization_constructor,
                                                bis);
    }
}

namespace {

    using namespace gorc;

    void store_palette_image(colormap const &cmp, path const &name)
    {
        // Palette is a 256 element grid = 16x16.
        image img(make_size(16, 16));
        size_t em = 0;
        for(size_t y = 0; y < 16; ++y) {
            for(size_t x = 0; x < 16; ++x, ++em) {
                img.get(x, y) = solid(cmp.palette->data[em]);
            }
        }

        save_image_to_file(img, name);
    }

    void store_light_level_image(colormap const &cmp, path const &name)
    {
        // Lighting is a 256x64 table with a 256x1 header
        image img(make_size(256, 66));

        for(size_t x = 0; x < 256; ++x) {
            img.get(x, 0) = solid(cmp.palette->data[x]);
        }

        for(size_t x = 0; x < 256; ++x) {
            // Border
            img.get(x, 1) = make_color_rgba8(255, 255, 255, 255);
        }

        size_t light_level = 0;
        for(size_t y = 2; y < 66; ++y, ++light_level) {
            for(size_t x = 0; x < 256; ++x) {
                auto color_index = cmp.light_levels->data[light_level][x];
                img.get(x, y) = solid(cmp.palette->data[color_index]);
            }
        }

        save_image_to_file(img, name);
    }

    void store_transparency_image(colormap const &cmp, path const &name)
    {
        // Transparency is a 256x256 table with a left header and a top header
        image img(make_size(258, 258));

        auto solid_white = make_color_rgba8(255, 255, 255, 255);

        img.get(0, 0) = solid_white;
        img.get(0, 1) = solid_white;
        img.get(1, 0) = solid_white;
        img.get(1, 1) = solid_white;

        for(size_t x = 0; x < 256; ++x) {
            auto col = solid(cmp.palette->data[x]);
            img.get(x + 2, 0) = col;
            img.get(x + 2, 1) = solid_white;

            img.get(0, x + 2) = col;
            img.get(1, x + 2) = solid_white;
        }

        for(size_t y = 0; y < 256; ++y) {
            for(size_t x = 0; x < 256; ++x) {
                auto col_ref = cmp.transparency_tables->data[y][x];
                auto col = solid(cmp.palette->data[col_ref]);

                img.get(x + 2, y + 2) = col;
            }
        }

        save_image_to_file(img, name);
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

        path extract_path = jos.services.get<gorc::extract_path>().value;

        path palette_path = extract_path;
        palette_path += "-palette.png";
        json_serialize_member(jos, "palette", [&]{
                json_serialize(jos, palette_path.generic_string()); });
        store_palette_image(*this, palette_path);

        path light_path = extract_path;
        light_path += "-light.png";
        json_serialize_member(jos, "light_levels", [&]{
                json_serialize(jos, light_path.generic_string()); });
        store_light_level_image(*this, light_path);

        if(transparency_tables) {
            path transparency_path = extract_path;
            transparency_path += "-transparency.png";
            json_serialize_member(jos, "transparency_tables", [&]{
                    json_serialize(jos, transparency_path.generic_string()); });
            store_transparency_image(*this, transparency_path);
        }
    });
}

gorc::color_rgb8 gorc::colormap::get_color(int index, int level) const
{
    return palette->data.at(light_levels->data.at(level).at(index));
}

gorc::color_rgb8 gorc::colormap::get_color(int index) const
{
    return get_color(index, 63);
}

gorc::color_rgb8 gorc::colormap::get_light_color(int index) const
{
    return get_color(index, 0);
}
