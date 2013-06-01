#include "nativefilesystem.h"
#include "framework/io/nativefile.h"

Gorc::Content::NativeFileSystem::NativeFileSystem(const boost::filesystem::path& basepath)
	: basepath(basepath) {
	return;
}

std::unique_ptr<Gorc::IO::ReadOnlyFile> Gorc::Content::NativeFileSystem::Open(const boost::filesystem::path& file) const {
	return std::unique_ptr<Gorc::IO::ReadOnlyFile>(new Gorc::IO::NativeFile(basepath / file));
}
