#include "material_loader.h"
#include "content/assets/material.h"
#include "framework/diagnostics/helper.h"
#include "framework/io/exception.h"
#include <array>
#include <boost/format.hpp>

const std::vector<boost::filesystem::path> Gorc::Content::Loaders::MaterialLoader::AssetRootPath = { "mat", "3do/mat" };

namespace Gorc {
namespace Content {
namespace Loaders {

struct MaterialHeader {
	uint32_t Type;
	int32_t MatRecordCount;
	int32_t TextureCount;
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

struct MaterialTextureDataHeader {
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

}
}
}

Gorc::Content::Loaders::MaterialLoader::MaterialLoader(const Assets::Colormap& Colormap)
	: Colormap(Colormap) {
	return;
}

std::unique_ptr<Gorc::Content::Asset> Gorc::Content::Loaders::MaterialLoader::Deserialize(IO::ReadOnlyFile& file, Manager& manager, Diagnostics::Report& report) {
	std::unique_ptr<Content::Assets::Material> mat(new Content::Assets::Material());

	char magic[4];
	file.Read(magic, sizeof(char) * 4);

	// Check magic and version
	if(strncmp(magic, "MAT ", 4) != 0 || file.Read<uint32_t>() != 0x32) {
		Diagnostics::Helper::FileCorrupt(report, "ColormapLoader::Deserialize",
				Diagnostics::ErrorLocation(file.Filename, 0, 0, 0, 0));
		throw IO::FileCorruptException();
	}

	MaterialHeader header;
	file.Read(&header, sizeof(MaterialHeader));

	if(header.BitDepth != 8) {
		report.AddError("MaterialLoader::Deserialize", boost::str(boost::format("%d bit materials not supported") % header.BitDepth),
				Diagnostics::ErrorLocation(file.Filename, 0, 0, 0, 0));
		throw IO::FileCorruptException();
	}

	if(header.Type == 0) {
		// Color mat
		mat->Width = 16;
		mat->Height = 16;

		for(int32_t i = 0; i < header.MatRecordCount; ++i) {
			MaterialColorRecordHeader colorRecord;
			file.Read(&colorRecord, sizeof(MaterialColorRecordHeader));

			Math::Vector<3, uint8_t> color, lightcolor;

			if(header.BitDepth == 8) {
				color = Colormap.GetColor(static_cast<uint8_t>(colorRecord.ColorNumber));
				lightcolor = Colormap.GetExtra(static_cast<uint8_t>(colorRecord.ColorNumber));
			}

			// Use 16x16 as a safe default texture size for color mats.
			size_t bufsz = 16 * 16 * 4;
			uint8_t buffer[bufsz];

			for(uint8_t* buf_idx = buffer; buf_idx < buffer + bufsz; buf_idx += 4) {
				buf_idx[0] = Math::Get<0>(color);
				buf_idx[1] = Math::Get<1>(color);
				buf_idx[2] = Math::Get<2>(color);
				buf_idx[3] = 255;
			}

			std::unique_ptr<sf::Image> diffuse(new sf::Image());
			if(!diffuse->LoadFromPixels(16, 16, buffer)) {
				report.AddCriticalError("MaterialLoader::Deserialize", "could not load diffuse color from pixels");
			}

			for(uint8_t* buf_idx = buffer; buf_idx < buffer + bufsz; buf_idx += 4) {
				buf_idx[0] = Math::Get<0>(lightcolor);
				buf_idx[1] = Math::Get<1>(lightcolor);
				buf_idx[2] = Math::Get<2>(lightcolor);
				buf_idx[3] = 255;
			}

			std::unique_ptr<sf::Image> light(new sf::Image());
			if(!light->LoadFromPixels(16, 16, buffer)) {
				report.AddCriticalError("MaterialLoader::Deserialize", "could not load emissive color from pixels");
			}

			mat->Cels.emplace_back(diffuse, light);
		}
	}
	else if(header.Type == 2) {
		// Texture mat
		std::vector<MaterialRecordHeader> record_headers(header.MatRecordCount);

		for(int32_t i = 0; i < header.MatRecordCount; ++i) {
			file.Read(&record_headers[i], sizeof(MaterialRecordHeader));
		}

		uint32_t bytesPerPixel = header.BitDepth >> 3;

		for(int32_t i = 0; i < header.MatRecordCount; ++i) {
			MaterialTextureDataHeader dataheader;
			file.Read(&dataheader, sizeof(MaterialTextureDataHeader));

			mat->Width = dataheader.SizeX;
			mat->Height = dataheader.SizeY;

			std::unique_ptr<sf::Image> diffuse(new sf::Image());
			std::unique_ptr<sf::Image> light(new sf::Image());

			if(header.BitDepth == 8) {
				// Process 8-bit texture with extra light information.
				std::vector<uint8_t> orig_buffer(dataheader.SizeX * dataheader.SizeY * bytesPerPixel);
				file.Read(&orig_buffer[0], orig_buffer.size() * sizeof(uint8_t));

				std::vector<uint8_t> col_buffer(dataheader.SizeX * dataheader.SizeY * sizeof(uint8_t) * 4);

				for(auto it = orig_buffer.begin(), jt = col_buffer.begin(); it != orig_buffer.end() && jt != col_buffer.end(); ++it, jt += 4) {
					auto color = Colormap.GetColor(*it);
					*(jt + 0) = Math::Get<0>(color);
					*(jt + 1) = Math::Get<1>(color);
					*(jt + 2) = Math::Get<2>(color);

					if(dataheader.UseTransparency && (*it == record_headers[i].TransparentColor || *it == header.Transparency)) {
						*(jt + 3) = 0;
					}
					else {
						*(jt + 3) = 255;
					}
				}

				diffuse->LoadFromPixels(dataheader.SizeX, dataheader.SizeY, &col_buffer[0]);

				for(auto it = orig_buffer.begin(), jt = col_buffer.begin(); it != orig_buffer.end() && jt != col_buffer.end(); ++it, jt += 4) {
					auto color = Colormap.GetExtra(*it);
					*(jt + 0) = Math::Get<0>(color);
					*(jt + 1) = Math::Get<1>(color);
					*(jt + 2) = Math::Get<2>(color);
					*(jt + 2) = 255;
				}

				light->LoadFromPixels(dataheader.SizeX, dataheader.SizeY, &col_buffer[0]);

				mat->Cels.emplace_back(diffuse, light);

				// Skip past mipmaps:
				for(size_t np = 1; np < dataheader.MipmapCount; ++np) {
					dataheader.SizeX >>= 1;
					dataheader.SizeY >>= 1;

					file.Seek(dataheader.SizeX * dataheader.SizeY * bytesPerPixel);
				}
			}
		}
	}

	return std::unique_ptr<Asset>(std::move(mat));
}
