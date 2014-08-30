#include "colormap_loader.h"
#include "content/assets/colormap.h"
#include "base/diagnostics/helper.h"
#include "base/io/exception.h"
#include "base/io/binary_input_stream.h"

const std::vector<boost::filesystem::path> gorc::content::loaders::colormap_loader::asset_root_path = { "misc/cmp", "misc" };

std::unique_ptr<gorc::content::asset> gorc::content::loaders::colormap_loader::deserialize(const boost::filesystem::path& filename,
        io::read_only_file& file, content_manager&, diagnostics::report& report) {
    std::unique_ptr<content::assets::colormap> cmp(new content::assets::colormap());

    char magic[4];
    file.read(magic, sizeof(char) * 4);

    // Check magic and version
    if(strncmp(magic, "CMP ", 4) != 0 || io::deserialize<uint32_t>(file) != 0x1E) {
        diagnostics::helper::file_corrupt(report, "ColormapLoader::Deserialize",
                diagnostics::error_location(filename, 0, 0, 0, 0));
        throw io::file_corrupt_exception();
    }

    bool hasTransparency = (io::deserialize<uint32_t>(file) != 0);

    get<0>(cmp->tint_color) = io::deserialize<float>(file);
    get<1>(cmp->tint_color) = io::deserialize<float>(file);
    get<2>(cmp->tint_color) = io::deserialize<float>(file);

    file.seek(40); // Skip padding/unknowns.

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
