#include "gobfile.h"
#include "framework/io/exception.h"

Gorc::Content::VFS::Gob::GobFile::GobFile(const boost::filesystem::path& gobPath,
		const boost::filesystem::path& filename, size_t chunkOffset, size_t chunkLength)
	: IO::ReadOnlyFile(filename), file(gobPath, false), chunkOffset(chunkOffset),
	  chunkLength(chunkLength), chunkEnd(chunkOffset + chunkLength) {
	file.SetPosition(chunkOffset);
	return;
}

void Gorc::Content::VFS::Gob::GobFile::Read(void* dest, size_t size) {
	if(GetPosition() + size > chunkLength) {
		throw IO::FileReadErrorException();
	}

	file.Read(dest, size);
}

void Gorc::Content::VFS::Gob::GobFile::Seek(long offset) {
	size_t currentPosition = GetPosition();
	if((offset < 0 && static_cast<size_t>(std::abs(offset)) > currentPosition)
			|| currentPosition + offset >= chunkEnd) {
		throw IO::FileInvalidSeekException();
	}

	file.Seek(offset);
}

void Gorc::Content::VFS::Gob::GobFile::SetPosition(size_t offset) {
	if(offset > chunkLength) {
		throw IO::FileInvalidSeekException();
	}

	file.SetPosition(chunkOffset + offset);
}

size_t Gorc::Content::VFS::Gob::GobFile::GetPosition() {
	return file.GetPosition() - chunkOffset;
}

size_t Gorc::Content::VFS::Gob::GobFile::GetSize() {
	return chunkLength;
}

bool Gorc::Content::VFS::Gob::GobFile::IsEndOfFile() {
	return GetPosition() >= chunkLength;
}
