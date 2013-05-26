#include "gobcontainer.h"
#include "content/vfs/exception.h"
#include <cstdint>
#include <cstddef>

Gorc::Content::VFS::Gob::GobContainer::GobContainer(IO::ReadOnlyFile& file)
	: gobPath(file.Filename), isEpisode(false), episodeIndex(0) {

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

	file.Read(&header, sizeof(GOB_HEADER));

	if(strncmp(header.magic, "GOB ", 4) != 0) {
		throw ContainerFileCorruptException();
	}

	for(uint32_t i = 0; i < header.indexCount; ++i) {
		file.Read(&entry, sizeof(GOB_ENTRY));

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

size_t Gorc::Content::VFS::Gob::GobContainer::FileCount() const {
	return files.size();
}

bool Gorc::Content::VFS::Gob::GobContainer::IsEpisode() const {
	return isEpisode;
}

const Gorc::Content::VFS::VirtualFile& Gorc::Content::VFS::Gob::GobContainer::GetEpisode() const {
	return GetVirtualFile(episodeIndex);
}

const Gorc::Content::VFS::VirtualFile& Gorc::Content::VFS::Gob::GobContainer::GetVirtualFile(size_t index) const {
	return files[index];
}
