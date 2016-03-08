#include "sound_loader.hpp"
#include "io/memory_file.hpp"
#include "libold/base/content/assets/sound.hpp"
#include <vector>
#include <cstdint>

const std::vector<boost::filesystem::path> gorc::content::loaders::sound_loader::asset_root_path = { "sound", "voice" };

std::unique_ptr<gorc::asset> gorc::content::loaders::sound_loader::deserialize(
        input_stream &file,
        content_manager &,
        service_registry const &) const
{
    std::unique_ptr<content::assets::sound> wav(new content::assets::sound());

    memory_file wav_buf;
    file.copy_to(wav_buf);

    wav->buffer.loadFromMemory(wav_buf.data(), wav_buf.size());

    return std::unique_ptr<asset>(std::move(wav));
}
