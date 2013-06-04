#include "colormap_loader.h"
#include "content/assets/colormap.h"
#include "framework/diagnostics/helper.h"
#include "framework/io/exception.h"

std::unique_ptr<Gorc::Content::Asset> Gorc::Content::Loaders::ColormapLoader::Deserialize(IO::ReadOnlyFile& file, Manager& manager, Diagnostics::Report& report) {
	std::unique_ptr<Content::Assets::Colormap> cmp(new Content::Assets::Colormap());

	char magic[4];
	file.Read(magic, sizeof(char) * 4);

	// Check magic and version
	if(strncmp(magic, "CMP ", 4) != 0 || file.Read<uint32_t>() != 0x1E) {
		Diagnostics::Helper::FileCorrupt(report, "ColormapLoader::Deserialize",
				Diagnostics::ErrorLocation(file.Filename, 0, 0, 0, 0));
		throw IO::FileCorruptException();
	}

	bool hasTransparency = (file.Read<uint32_t>() != 0);

	file.Seek(52); // Skip padding/unknowns.

	// Read color tables, 24-bit RGB.
	uint8_t colorbytes[768];
	file.Read(colorbytes, sizeof(uint8_t) * 768);

	uint8_t* cb_idx = colorbytes;
	for(size_t i = 0; i < 256; ++i, cb_idx += 3) {
		cmp->SetColor(i, Math::Vec(cb_idx[0], cb_idx[1], cb_idx[2]));
	}

	// Read first (darkest) light level table to generate texture light data.
	uint8_t lightbytes[256];
	file.Read(lightbytes, sizeof(uint8_t) * 256);
	for(size_t i = 0; i < 256; ++i) {
		cmp->SetExtra(i, cmp->GetColor(lightbytes[i]));
	}

	return std::unique_ptr<Asset>(std::move(cmp));
}
