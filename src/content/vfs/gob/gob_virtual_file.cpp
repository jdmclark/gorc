#include "gob_virtual_file.h"
#include "gob_container.h"
#include "gob_file.h"

Gorc::Content::VFS::Gob::GobVirtualFile::GobVirtualFile(size_t chunkOffset,
		size_t chunkLength, const boost::filesystem::path& filename, const GobContainer& parentContainer)
	: chunkOffset(chunkOffset), chunkLength(chunkLength), filename(filename), parentContainer(parentContainer) {
	return;
}

const boost::filesystem::path& Gorc::Content::VFS::Gob::GobVirtualFile::GetFilename() const {
	return filename;
}

std::unique_ptr<Gorc::IO::ReadOnlyFile> Gorc::Content::VFS::Gob::GobVirtualFile::Open() const {
	return std::unique_ptr<Gorc::IO::ReadOnlyFile>(new GobFile(parentContainer.GetPath(),
			filename, chunkOffset, chunkLength));
}
