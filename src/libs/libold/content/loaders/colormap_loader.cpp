#include <cstdint>
#include "log/log.hpp"
#include "colormap_loader.hpp"
#include "libold/content/assets/colormap.hpp"
#include "io/binary_input_stream.hpp"

const std::vector<boost::filesystem::path> gorc::content::loaders::colormap_loader::asset_root_path = { "misc/cmp" };

std::unique_ptr<gorc::asset> gorc::content::loaders::colormap_loader::deserialize(
        input_stream &file,
        content_manager&,
        service_registry const &) const
{
    binary_input_stream bis(file);
    std::unique_ptr<content::assets::colormap> cmp(new content::assets::colormap());

    char magic[4];
    file.read(magic, sizeof(char) * 4);

    // Check magic and version
    if(strncmp(magic, "CMP ", 4) != 0 || binary_deserialize<uint32_t>(bis) != 0x1E) {
        LOG_FATAL("file is not a valid CMP");
    }

    binary_deserialize<uint32_t>(bis);

    get<0>(cmp->tint_color) = binary_deserialize<float>(bis);
    get<1>(cmp->tint_color) = binary_deserialize<float>(bis);
    get<2>(cmp->tint_color) = binary_deserialize<float>(bis);

    // Skip padding/unknowns
    std::array<char, 40> unknowns;
    file.read(unknowns.data(), sizeof(unknowns));

    // Read color tables, 24-bit RGB.
    uint8_t colorbytes[768];
    file.read(colorbytes, sizeof(uint8_t) * 768);

    uint8_t* cb_idx = colorbytes;
    for(size_t i = 0; i < 256; ++i, cb_idx += 3) {
        cmp->set_color(static_cast<uint8_t>(i), make_vector(cb_idx[0], cb_idx[1], cb_idx[2]));
    }

    // Read first (darkest) light level table to generate texture light data.
    uint8_t lightbytes[256];
    file.read(lightbytes, sizeof(uint8_t) * 256);
    for(size_t i = 0; i < 256; ++i) {
        cmp->set_extra(static_cast<uint8_t>(i), cmp->get_color(lightbytes[static_cast<uint8_t>(i)]));
    }

    return std::unique_ptr<asset>(std::move(cmp));
}
