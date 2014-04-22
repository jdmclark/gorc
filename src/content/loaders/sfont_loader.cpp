#include "sfont_loader.h"
#include "content/assets/sfont.h"
#include "bitmap_loader.h"
#include "base/diagnostics/helper.h"
#include "base/io/exception.h"
#include "base/io/binary_input_stream.h"

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

gorc::content::loaders::sfont_loader::sfont_loader(const assets::colormap& colormap)
    : pal_colormap(&colormap) {
    return;
}

gorc::content::loaders::sfont_loader::sfont_loader(const assets::bitmap& bitmap)
    : pal_bitmap(&bitmap) {
    return;
}

const std::vector<boost::filesystem::path> gorc::content::loaders::sfont_loader::asset_root_path = { "ui/sft" };

std::unique_ptr<gorc::content::asset> gorc::content::loaders::sfont_loader::deserialize(const boost::filesystem::path& filename,
        io::read_only_file& file, content_manager& manager, diagnostics::report& report) {
    std::unique_ptr<content::assets::sfont> cmp(new content::assets::sfont());

    sfont_header header;
    file.read(&header, sizeof(sfont_header));

    // Check fourcc
    if(strncmp(header.fourcc, "SFNT", 4) != 0) {
        diagnostics::helper::file_corrupt(report, "sfont_loader::deserialize",
                diagnostics::error_location(filename, 0, 0, 0, 0));
        throw io::file_corrupt_exception();
    }

    std::vector<std::tuple<char, int, int>> working_glyphs;

    for(unsigned int i = 0; i < header.num_tables; ++i) {
        uint16_t first_char = io::deserialize<uint16_t>(file);
        uint16_t last_char = io::deserialize<uint16_t>(file);

        for(; first_char <= last_char; ++first_char) {
            // TODO: Update this code (and the font base class) to support unicode fonts.
            int xoff = io::deserialize<int32_t>(file);
            int width = io::deserialize<int32_t>(file);
            working_glyphs.emplace_back(static_cast<char>(first_char), xoff, width);
        }
    }

    bitmap_loader embedded_bm_loader = pal_colormap ? bitmap_loader(*pal_colormap) : bitmap_loader(*pal_bitmap);
    cmp->uncast_embedded_bm = embedded_bm_loader.deserialize(filename, file, manager, report);
    cmp->embedded_bm = dynamic_cast<assets::bitmap*>(cmp->uncast_embedded_bm.get());
    if(!cmp->embedded_bm) {
        diagnostics::helper::file_corrupt(report, "sfont_loader::deserialize",
                diagnostics::error_location(filename, 0, 0, 0, 0));
        throw io::file_corrupt_exception();
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
