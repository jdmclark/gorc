#include "material_loader.h"
#include "content/assets/material.h"
#include "framework/diagnostics/helper.h"
#include "framework/io/exception.h"
#include <array>
#include <boost/format.hpp>

const std::vector<boost::filesystem::path> gorc::content::loaders::material_loader::asset_root_path = { "mat", "3do/mat" };

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

gorc::content::loaders::material_loader::material_loader(const assets::colormap& colormap)
	: colormap(colormap) {
	return;
}

std::unique_ptr<gorc::content::asset> gorc::content::loaders::material_loader::deserialize(io::read_only_file& file, manager& manager, diagnostics::report& report) {
	std::unique_ptr<content::assets::material> mat(new content::assets::material());

	char magic[4];
	file.read(magic, sizeof(char) * 4);

	// Check magic and version
	if(strncmp(magic, "MAT ", 4) != 0 || file.read<uint32_t>() != 0x32) {
		diagnostics::helper::file_corrupt(report, "ColormapLoader::Deserialize",
				diagnostics::error_location(file.Filename, 0, 0, 0, 0));
		throw io::file_corrupt_exception();
	}

	MaterialHeader header;
	file.read(&header, sizeof(MaterialHeader));

	if(header.BitDepth != 8) {
		report.add_error("MaterialLoader::Deserialize", boost::str(boost::format("%d bit materials not supported") % header.BitDepth),
				diagnostics::error_location(file.Filename, 0, 0, 0, 0));
		throw io::file_corrupt_exception();
	}

	if(header.type == 0) {
		// Color mat
		mat->width = 16;
		mat->height = 16;

		for(int32_t i = 0; i < header.MatRecordCount; ++i) {
			MaterialColorRecordHeader colorRecord;
			file.read(&colorRecord, sizeof(MaterialColorRecordHeader));

			vector<3, uint8_t> color, lightcolor;

			if(header.BitDepth == 8) {
				color = colormap.get_color(static_cast<uint8_t>(colorRecord.ColorNumber));
				lightcolor = colormap.get_extra(static_cast<uint8_t>(colorRecord.ColorNumber));
			}

			// Use 16x16 as a safe default texture size for color mats.
			size_t bufsz = 16 * 16 * 4;
			uint8_t buffer[bufsz];

			for(uint8_t* buf_idx = buffer; buf_idx < buffer + bufsz; buf_idx += 4) {
				buf_idx[0] = get<0>(color);
				buf_idx[1] = get<1>(color);
				buf_idx[2] = get<2>(color);
				buf_idx[3] = 255;
			}

			auto diffuse = LoadMaterialFromMemory(16, 16, buffer);

			for(uint8_t* buf_idx = buffer; buf_idx < buffer + bufsz; buf_idx += 4) {
				buf_idx[0] = get<0>(lightcolor);
				buf_idx[1] = get<1>(lightcolor);
				buf_idx[2] = get<2>(lightcolor);
				buf_idx[3] = 255;
			}

			auto light = LoadMaterialFromMemory(16, 16, buffer);

			mat->cels.emplace_back(diffuse, light);
		}
	}
	else if(header.type == 2) {
		// texture mat
		std::vector<MaterialRecordHeader> record_headers(header.MatRecordCount);

		for(int32_t i = 0; i < header.MatRecordCount; ++i) {
			file.read(&record_headers[i], sizeof(MaterialRecordHeader));
		}

		uint32_t bytesPerPixel = header.BitDepth >> 3;

		for(int32_t i = 0; i < header.MatRecordCount; ++i) {
			MaterialtextureDataHeader dataheader;
			file.read(&dataheader, sizeof(MaterialtextureDataHeader));

			mat->width = dataheader.SizeX;
			mat->height = dataheader.SizeY;

			if(header.BitDepth == 8) {
				// Process 8-bit texture with extra light information.
				std::vector<uint8_t> orig_buffer(dataheader.SizeX * dataheader.SizeY * bytesPerPixel);
				file.read(&orig_buffer[0], orig_buffer.size() * sizeof(uint8_t));

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

				mat->cels.emplace_back(diffuse, light);

				// Skip past mipmaps:
				for(size_t np = 1; np < dataheader.MipmapCount; ++np) {
					dataheader.SizeX >>= 1;
					dataheader.SizeY >>= 1;

					file.seek(dataheader.SizeX * dataheader.SizeY * bytesPerPixel);
				}
			}
		}
	}

	return std::unique_ptr<asset>(std::move(mat));
}
