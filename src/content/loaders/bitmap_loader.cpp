#include "bitmap_loader.h"
#include "content/assets/bitmap.h"
#include "content/assets/colormap.h"
#include "base/diagnostics/helper.h"
#include "base/io/exception.h"
#include "base/math/util.h"
#include <array>
#include <boost/format.hpp>

const std::vector<boost::filesystem::path> gorc::content::loaders::bitmap_loader::asset_root_path = { "ui/bm" };

namespace gorc {
namespace content {
namespace loaders {

struct bitmap_header {
    int32_t unknown1;
    int32_t unknown2;

    uint32_t pal_included;
    uint32_t num_images;

    int32_t x_offset;
    int32_t y_offset;

    uint32_t transparent_color;

    int32_t unknown3;

    uint32_t bit_depth;

    uint32_t blue_bits;
    uint32_t green_bits;
    uint32_t red_bits;

    uint32_t red_shl;
    uint32_t green_shl;
    uint32_t blue_shl;

    uint32_t unknowns[16];
};

struct bitmap_image_header {
    uint32_t width;
    uint32_t height;
};

GLuint load_bitmap_from_memory(int width, int height, const uint8_t* data) {
    GLuint texture_id = GL_INVALID_VALUE;

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    return texture_id;
}

}
}
}

gorc::content::loaders::bitmap_loader::bitmap_loader(const assets::colormap& colormap,
                                                     bool disable_transparency)
    : pal_colormap(&colormap)
    , disable_transparency(disable_transparency) {
    return;
}

gorc::content::loaders::bitmap_loader::bitmap_loader(const assets::bitmap& bitmap,
                                                     bool disable_transparency)
    : pal_bitmap(&bitmap)
    , disable_transparency(disable_transparency) {
    return;
}

gorc::vector<3, uint8_t> gorc::content::loaders::bitmap_loader::get_palette_color(uint8_t index) const {
    if(pal_colormap) {
        return pal_colormap->get_color(index);
    }
    else if(pal_bitmap) {
        return pal_bitmap->get_color(index);
    }

    return make_zero_vector<3, uint8_t>();
}

std::unique_ptr<gorc::content::asset> gorc::content::loaders::bitmap_loader::deserialize(const boost::filesystem::path& filename,
        io::read_only_file& file, content_manager&, diagnostics::report& report) {
    std::unique_ptr<content::assets::bitmap> mat(new content::assets::bitmap());

    char magic[4];
    file.read(magic, sizeof(char) * 4);

    // Check magic and version
    if(strncmp(magic, "BM  ", 4) != 0) {
        diagnostics::helper::file_corrupt(report, "bitmap_loader::deserialize", diagnostics::error_location(filename, 0, 0, 0, 0));
        throw io::file_corrupt_exception();
    }

    bitmap_header header;
    file.read(&header, sizeof(bitmap_header));

    if(header.pal_included == 2) {
        // Palette is included at the end of the file.
        pal_bitmap = mat.get();
        pal_colormap = nullptr;
    }

    if(header.bit_depth == 8) {
        std::vector<std::tuple<int, int, std::vector<uint8_t>>> images;

        for(unsigned int i = 0; i < header.num_images; ++i) {
            bitmap_image_header bi_header;
            file.read(&bi_header, sizeof(bitmap_image_header));

            images.emplace_back(static_cast<int>(bi_header.width), static_cast<int>(bi_header.height),
                    std::vector<uint8_t>(bi_header.width * bi_header.height));
            auto& v = std::get<2>(images.back());
            file.read(&v[0], v.size() * sizeof(uint8_t));
        }

        // Load palette
        if(header.pal_included == 2) {
            uint8_t pal_buf[sizeof(uint8_t) * 3 * 256];
            file.read(pal_buf, sizeof(uint8_t) * 3 * 256);
            for(int i = 0, j = 0; i < 256; ++i, j += 3) {
                mat->set_color(static_cast<uint8_t>(i), make_vector(pal_buf[j], pal_buf[j + 1], pal_buf[j + 2]));
            }
        }

        // Create images
        for(auto& img : images) {
            int adjusted_width = next_power_of_two(std::get<0>(img));
            int adjusted_height = next_power_of_two(std::get<1>(img));
            int stride = (adjusted_width - std::get<0>(img) - 1) * 4;

            int num_pels = std::get<0>(img) * std::get<1>(img);
            std::vector<uint8_t> final_img(adjusted_width * adjusted_height * sizeof(uint8_t) * 4, 0);
            for(int i = 0, j = 0; i < num_pels; j += stride) {
                for(int k = 0; k < std::get<0>(img); ++k, ++i, j += 4) {
                    auto pal_idx = get<2>(img)[i];
                    auto pal_col = get_palette_color(pal_idx);

                    final_img[j + 0] = get<0>(pal_col);
                    final_img[j + 1] = get<1>(pal_col);
                    final_img[j + 2] = get<2>(pal_col);

                    if(!disable_transparency && pal_idx == header.transparent_color) {
                        final_img[j + 3] = 0;
                    }
                    else {
                        final_img[j + 3] = 255;
                    }
                }
            }

            mat->cels.emplace_back(load_bitmap_from_memory(adjusted_width, adjusted_height, &final_img[0]),
                    make_box(make_vector(0, 0), make_vector(adjusted_width, adjusted_height)),
                    make_box(make_vector(0, 0), make_vector(std::get<0>(img), std::get<1>(img))));
        }
    }
    else if(header.bit_depth == 16) {
        std::vector<std::tuple<int, int, std::vector<uint16_t>>> images;

        for(unsigned int i = 0; i < header.num_images; ++i) {
            bitmap_image_header bi_header;
            file.read(&bi_header, sizeof(bitmap_image_header));

            images.emplace_back(static_cast<int>(bi_header.width), static_cast<int>(bi_header.height),
                    std::vector<uint16_t>(bi_header.width * bi_header.height));
            auto& v = std::get<2>(images.back());
            file.read(&v[0], v.size() * sizeof(uint16_t));
        }

        // Load palette
        if(header.pal_included == 2) {
            uint8_t pal_buf[sizeof(uint8_t) * 3 * 256];
            file.read(pal_buf, sizeof(uint8_t) * 3 * 256);
            for(int i = 0, j = 0; i < 256; ++i, j += 3) {
                mat->set_color(static_cast<uint8_t>(i), make_vector(pal_buf[j], pal_buf[j + 1], pal_buf[j + 2]));
            }
        }

        // Create images
        for(auto& img : images) {
            int adjusted_width = next_power_of_two(std::get<0>(img));
            int adjusted_height = next_power_of_two(std::get<1>(img));
            int stride = (adjusted_width - std::get<0>(img)) * 4;

            int num_pels = std::get<0>(img) * std::get<1>(img);
            std::vector<uint8_t> final_img(adjusted_width * adjusted_height * sizeof(uint8_t) * 4, 0);
            for(int i = 0, j = 0; i < num_pels; j += stride) {
                for(int k = 0; k < std::get<0>(img); ++k, ++i, j += 4) {
                    auto pal_idx = get<2>(img)[i];
                    auto pal_col = convert_rgb565(pal_idx);

                    final_img[j + 0] = get<0>(pal_col);
                    final_img[j + 1] = get<1>(pal_col);
                    final_img[j + 2] = get<2>(pal_col);

                    if(!disable_transparency && pal_idx == header.transparent_color) {
                        final_img[j + 3] = 0;
                    }
                    else {
                        final_img[j + 3] = 255;
                    }
                }
            }

            auto bm_handle = load_bitmap_from_memory(adjusted_width, adjusted_height, &final_img[0]);
            mat->cels.emplace_back(bm_handle, make_box(make_vector(0, 0), make_vector(adjusted_width, adjusted_height)),
                    make_box(make_vector(0, 0), make_vector(std::get<0>(img), std::get<1>(img))));
        }
    }
    else {
        // Unknown bit depth
        diagnostics::helper::file_corrupt(report, "bitmap_loader::deserialize", diagnostics::error_location(filename, 0, 0, 0, 0));
        throw io::file_corrupt_exception();
    }

    return std::unique_ptr<asset>(std::move(mat));
}
