#include "sound_loader.h"
#include "content/assets/sound.h"
#include "framework/diagnostics/helper.h"
#include "framework/io/exception.h"
#include <vector>
#include <cstdint>

const std::vector<boost::filesystem::path> Gorc::Content::Loaders::SoundLoader::AssetRootPath = { "sound", "voice" };

std::unique_ptr<Gorc::Content::Asset> Gorc::Content::Loaders::SoundLoader::Deserialize(IO::ReadOnlyFile& file, Manager& manager, Diagnostics::Report& report) {
	std::unique_ptr<Content::Assets::Sound> wav(new Content::Assets::Sound());

	size_t wav_sz = file.GetSize();
	std::vector<char> wav_buf(wav_sz);
	file.Read(&wav_buf[0], wav_sz);

	wav->Buffer.loadFromMemory(&wav_buf[0], wav_sz);

	return std::unique_ptr<Asset>(std::move(wav));
}
