#include "log/log.hpp"
#include "sfont_loader.hpp"
#include "libold/content/assets/sfont.hpp"
#include "bitmap_loader.hpp"
#include "io/binary_input_stream.hpp"

namespace gorc {
namespace content {
namespace loaders {

struct sfont_header {
    char fourcc[4];
    int32_t unknown1[4];
    uint32_t num_tables;
    int32_t unknown2[4];
};

}
}
}

gorc::content::loaders::sfont_loader::sfont_loader(const assets::colormap& colormap,
                                                   bool disable_transparency)
    : pal_colormap(&colormap)
    , disable_transparency(disable_transparency) {
    return;
}

gorc::content::loaders::sfont_loader::sfont_loader(const assets::bitmap& bitmap,
                                                   bool disable_transparency)
    : pal_bitmap(&bitmap)
    , disable_transparency(disable_transparency) {
    return;
}

namespace {
    const std::vector<gorc::path> asset_root_path = { "ui/sft" };
}

std::vector<gorc::path> const& gorc::content::loaders::sfont_loader::get_prefixes() const
{
    return asset_root_path;
}

std::unique_ptr<gorc::asset> gorc::content::loaders::sfont_loader::deserialize(
        input_stream& file, content_manager& manager, service_registry const &services) const {
    binary_input_stream bis(file);
    std::unique_ptr<content::assets::sfont> cmp(new content::assets::sfont());

    sfont_header header;
    bis.read(&header, sizeof(sfont_header));

    // Check fourcc
    if(strncmp(header.fourcc, "SFNT", 4) != 0) {
        LOG_FATAL("file is not a valid SFNT");
    }

    std::vector<std::tuple<char, int, int>> working_glyphs;

    for(unsigned int i = 0; i < header.num_tables; ++i) {
        uint16_t first_char = binary_deserialize<uint16_t>(bis);
        uint16_t last_char = binary_deserialize<uint16_t>(bis);

        for(; first_char <= last_char; ++first_char) {
            // TODO: Update this code (and the font base class) to support unicode fonts.
            int xoff = binary_deserialize<int32_t>(bis);
            int width = binary_deserialize<int32_t>(bis);
            working_glyphs.emplace_back(static_cast<char>(first_char), xoff, width);
        }
    }

    bitmap_loader embedded_bm_loader = pal_colormap ?
                                       bitmap_loader(*pal_colormap, disable_transparency) :
                                       bitmap_loader(*pal_bitmap, disable_transparency);
    cmp->uncast_embedded_bm = embedded_bm_loader.deserialize(file, manager, services);
    cmp->embedded_bm = dynamic_cast<assets::bitmap*>(cmp->uncast_embedded_bm.get());
    if(!cmp->embedded_bm) {
        LOG_FATAL("could not load embedded BM");
    }

    cmp->height = get_size<1>(cmp->embedded_bm->cels.front().original_size);

    float sft_tex_width = static_cast<float>(get_size<0>(cmp->embedded_bm->cels.front().color.size()));
    float sft_tex_height = static_cast<float>(cmp->height) / static_cast<float>(get_size<1>(cmp->embedded_bm->cels.front().color.size()));

    for(auto& g : working_glyphs) {
        char gc;
        int gxoff, gwidth;
        std::tie(gc, gxoff, gwidth) = g;
        cmp->glyph_map.emplace(gc, assets::glyph(
                make_box(make_vector(0, 0), make_vector(gwidth, cmp->height)),
                make_box(make_vector(static_cast<float>(gxoff) / sft_tex_width, 0.0f),
                        make_vector(static_cast<float>(gxoff + gwidth) / sft_tex_width, sft_tex_height))));
    }

    return std::unique_ptr<asset>(std::move(cmp));
}
