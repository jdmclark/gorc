#include "sound_loader.h"
#include "base/content/assets/sound.h"
#include "base/diagnostics/helper.h"
#include "base/io/exception.h"
#include <vector>
#include <cstdint>

const std::vector<boost::filesystem::path> gorc::content::loaders::sound_loader::asset_root_path = { "sound", "voice" };

std::unique_ptr<gorc::content::asset> gorc::content::loaders::sound_loader::deserialize(const boost::filesystem::path&,
        io::read_only_file& file, content_manager&, diagnostics::report&) {
    std::unique_ptr<content::assets::sound> wav(new content::assets::sound());

    size_t wav_sz = file.size();
    std::vector<char> wav_buf(wav_sz);
    file.read(&wav_buf[0], wav_sz);

    wav->buffer.loadFromMemory(&wav_buf[0], wav_sz);

    return std::unique_ptr<asset>(std::move(wav));
}
