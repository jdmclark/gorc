#include <cstdint>
#include "log/log.hpp"
#include "material_loader.hpp"
#include "libold/content/assets/material.hpp"
#include "libold/content/master_colormap.hpp"
#include "io/binary_input_stream.hpp"
#include <array>
#include <boost/format.hpp>

gorc::fourcc const gorc::content::loaders::material_loader::type = "MAT"_4CC;

namespace {
    const std::vector<gorc::path> asset_root_path = { "mat", "3do/mat" };
}

std::vector<gorc::path> const& gorc::content::loaders::material_loader::get_prefixes() const
{
    return asset_root_path;
}

namespace gorc {
namespace content {
namespace loaders {

struct MaterialHeader {
    uint32_t type;
    int32_t MatRecordCount;
    int32_t textureCount;
    uint32_t Transparency;
    uint32_t BitDepth;

    uint32_t BlueBits;
    uint32_t RedBits;
    uint32_t GreenBits;

    uint32_t RedShl;
    uint32_t GreenShl;
    uint32_t BlueShl;

    uint32_t RedShr;
    uint32_t GreenShr;
    uint32_t BlueShr;

    uint32_t Unknown1;
    uint32_t Unknown2;
    uint32_t Unknown3;
};

struct MaterialRecordHeader {
    uint32_t RecordType;
    uint32_t TransparentColor;
    uint32_t Unknown1;
    uint32_t Unknown2;
    uint32_t Unknown3;
    uint32_t Unknown4;
    uint32_t Unknown5;
    uint32_t Unknown6;
    uint32_t Unknown7;
    uint32_t Unknown8;
};

struct MaterialtextureDataHeader {
    int32_t SizeX;
    int32_t SizeY;
    uint32_t UseTransparency;

    uint32_t Unknown1;
    uint32_t Unknown2;

    uint32_t MipmapCount;
};

struct MaterialColorRecordHeader {
    uint32_t RecordType;
    uint32_t ColorNumber;

    uint32_t Unknown1;
    uint32_t Unknown2;
    uint32_t Unknown3;
    uint32_t Unknown4;
};

GLuint LoadMaterialFromMemory(unsigned int width, unsigned int height, const uint8_t* data) {
    GLuint texture_id;

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLfloat largest_supported_anisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest_supported_anisotropy);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    return texture_id;
}

}
}
}

std::unique_ptr<gorc::asset> gorc::content::loaders::material_loader::deserialize(
        input_stream& file, content_manager&, service_registry const &services) const {
    LOG_FATAL_ASSERT(services.get<master_colormap>().cmp.has_value(),
                     "loaded material, but no master colormap is set");
    auto const &colormap = *services.get<master_colormap>().cmp.get_value();

    binary_input_stream bis(file);
    std::unique_ptr<content::assets::material> mat(new content::assets::material());

    char magic[4];
    bis.read(magic, sizeof(char) * 4);

    // Check magic and version
    if(strncmp(magic, "MAT ", 4) != 0 || binary_deserialize<uint32_t>(bis) != 0x32) {
        LOG_FATAL("file is not a valid MAT");
    }

    MaterialHeader header;
    bis.read(&header, sizeof(MaterialHeader));

    if(header.BitDepth != 8) {
        LOG_FATAL(format("bitdepth %d is not supported") % header.BitDepth);
    }

    if(header.type == 0) {
        // Color mat
        mat->size = make_box(make_vector(0, 0), make_vector(16, 16));

        for(int32_t i = 0; i < header.MatRecordCount; ++i) {
            MaterialColorRecordHeader colorRecord;
            bis.read(&colorRecord, sizeof(MaterialColorRecordHeader));

            vector<3, uint8_t> color = make_zero_vector<3, uint8_t>();
            vector<3, uint8_t> lightcolor = make_zero_vector<3, uint8_t>();

            if(header.BitDepth == 8) {
                color = colormap.get_color(static_cast<uint8_t>(colorRecord.ColorNumber));
                lightcolor = colormap.get_extra(static_cast<uint8_t>(colorRecord.ColorNumber));
            }

            // Use 16x16 as a safe default texture size for color mats.
            constexpr size_t bufsz = 16 * 16 * 4;
            uint8_t buffer[bufsz];

            for(size_t i = 0; i < bufsz; ++i) {
                switch(i & 0x3) {
                case 0x0:
                    buffer[i] = get<0>(color);
                    break;

                case 0x1:
                    buffer[i] = get<1>(color);
                    break;

                case 0x2:
                    buffer[i] = get<2>(color);
                    break;

                case 0x3:
                    buffer[i] = 255U;
                    break;
                }
            }

            for(size_t i = 0; i < bufsz; i += 4) {
                uint8_t* buf_idx = &buffer[i];
                buf_idx[0] = get<0>(color);
                buf_idx[1] = get<1>(color);
                buf_idx[2] = get<2>(color);
                buf_idx[3] = 255;
            }

            auto diffuse = LoadMaterialFromMemory(16, 16, buffer);

            for(size_t i = 0; i < bufsz; i += 4) {
                uint8_t* buf_idx = &buffer[i];
                buf_idx[0] = get<0>(lightcolor);
                buf_idx[1] = get<1>(lightcolor);
                buf_idx[2] = get<2>(lightcolor);
                buf_idx[3] = 255;
            }

            auto light = LoadMaterialFromMemory(16, 16, buffer);

            mat->cels.emplace_back(diffuse, light, mat->size);
        }
    }
    else if(header.type == 2) {
        // texture mat
        std::vector<MaterialRecordHeader> record_headers(header.MatRecordCount);

        for(int32_t i = 0; i < header.MatRecordCount; ++i) {
            bis.read(&record_headers[i], sizeof(MaterialRecordHeader));
        }

        uint32_t bytesPerPixel = header.BitDepth >> 3;

        for(int32_t i = 0; i < header.MatRecordCount; ++i) {
            MaterialtextureDataHeader dataheader;
            bis.read(&dataheader, sizeof(MaterialtextureDataHeader));

            mat->size = make_box(make_vector(0, 0), make_vector(dataheader.SizeX, dataheader.SizeY));

            if(header.BitDepth == 8) {
                // Process 8-bit texture with extra light information.
                std::vector<uint8_t> orig_buffer(dataheader.SizeX * dataheader.SizeY * bytesPerPixel);
                bis.read(&orig_buffer[0], orig_buffer.size() * sizeof(uint8_t));

                std::vector<uint8_t> col_buffer(dataheader.SizeX * dataheader.SizeY * sizeof(uint8_t) * 4);

                for(auto it = orig_buffer.begin(), jt = col_buffer.begin(); it != orig_buffer.end() && jt != col_buffer.end(); ++it, jt += 4) {
                    auto color = colormap.get_color(*it);
                    *(jt + 0) = get<0>(color);
                    *(jt + 1) = get<1>(color);
                    *(jt + 2) = get<2>(color);

                    if(dataheader.UseTransparency && (*it == record_headers[i].TransparentColor || *it == header.Transparency)) {
                        *(jt + 3) = 0;
                    }
                    else {
                        *(jt + 3) = 255;
                    }
                }

                auto diffuse = LoadMaterialFromMemory(dataheader.SizeX, dataheader.SizeY, col_buffer.data());

                for(auto it = orig_buffer.begin(), jt = col_buffer.begin(); it != orig_buffer.end() && jt != col_buffer.end(); ++it, jt += 4) {
                    auto color = colormap.get_extra(*it);
                    *(jt + 0) = get<0>(color);
                    *(jt + 1) = get<1>(color);
                    *(jt + 2) = get<2>(color);
                    *(jt + 3) = 255;
                }

                auto light = LoadMaterialFromMemory(dataheader.SizeX, dataheader.SizeY, col_buffer.data());

                mat->cels.emplace_back(diffuse, light, mat->size);

                // Skip past mipmaps:
                std::vector<char> scratch;
                for(size_t np = 1; np < dataheader.MipmapCount; ++np) {
                    dataheader.SizeX >>= 1;
                    dataheader.SizeY >>= 1;

                    scratch.resize(dataheader.SizeX * dataheader.SizeY * bytesPerPixel);
                    file.read(&scratch[0], scratch.size());
                }
            }
        }
    }

    return std::unique_ptr<asset>(std::move(mat));
}
