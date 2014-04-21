#include "gob_container.h"
#include "content/vfs/exception.h"
#include <cstdint>
#include <cstddef>

gorc::content::vfs::gob::gob_container::gob_container(const boost::filesystem::path& gobPath, io::read_only_file& file)
    : gobPath(gobPath), isEpisode(false), episodeIndex(0) {

    struct GOB_HEADER {
        char magic[4];
        uint32_t firstIndexOffset;
        uint32_t unknown;
        uint32_t indexCount;
    } header;

    struct GOB_ENTRY {
        uint32_t chunkOffset;
        uint32_t chunkLength;
        char chunkName[128];
    } entry;

    file.read(&header, sizeof(GOB_HEADER));

    if(strncmp(header.magic, "GOB ", 4) != 0) {
        throw container_file_corrupt_exception();
    }

    for(uint32_t i = 0; i < header.indexCount; ++i) {
        file.read(&entry, sizeof(GOB_ENTRY));

        // Convert path separators for boost path.
        for(size_t i = 0; i < 128; ++i) {
            if(entry.chunkName[i] == '\\') {
                entry.chunkName[i] = '/';
            }
        }

        entry.chunkName[127] = '\0';

        if(strncasecmp(entry.chunkName, "episode.jk", 11) == 0) {
            isEpisode = true;
            episodeIndex = files.size();
        }

        files.emplace_back(entry.chunkOffset, entry.chunkLength, entry.chunkName, *this);
    }
}

size_t gorc::content::vfs::gob::gob_container::file_count() const {
    return files.size();
}

bool gorc::content::vfs::gob::gob_container::is_episode() const {
    return isEpisode;
}

const gorc::content::vfs::virtual_file& gorc::content::vfs::gob::gob_container::get_episode() const {
    return get_virtual_file(episodeIndex);
}

const gorc::content::vfs::virtual_file& gorc::content::vfs::gob::gob_container::get_virtual_file(size_t index) const {
    return files[index];
}
