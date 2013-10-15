#include "gob_virtual_file.h"
#include "gob_container.h"
#include "gob_file.h"

gorc::content::vfs::gob::gob_virtual_file::gob_virtual_file(size_t chunkOffset,
		size_t chunkLength, const boost::filesystem::path& filename, const gob_container& parentContainer)
	: chunkOffset(chunkOffset), chunkLength(chunkLength), filename(filename), parentContainer(parentContainer) {
	return;
}

const boost::filesystem::path& gorc::content::vfs::gob::gob_virtual_file::get_filename() const {
	return filename;
}

std::unique_ptr<gorc::io::read_only_file> gorc::content::vfs::gob::gob_virtual_file::open() const {
	return std::unique_ptr<gorc::io::read_only_file>(new gob_file(parentContainer.get_path(),
			filename, chunkOffset, chunkLength));
}
