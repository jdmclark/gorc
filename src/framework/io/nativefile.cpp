#include "nativefile.h"
#include "exception.h"

#include <errno.h>

using namespace Gorc::IO;

NativeFile::NativeFile(const boost::filesystem::path& filename, bool createnew)
	: File(filename.native()) {
	hFile = nullptr;

	if(createnew) {
		if(create(filename)) {
			return;
		}

		throw FileCreateErrorException();
	}
	else {
		if(open(filename)) {
			return;
		}

		throw FileOpenErrorException();
	}

	return;
}

NativeFile::~NativeFile() {
	close();
	return;
}

bool NativeFile::open(const boost::filesystem::path& filename) {
	if(hFile) {
		if(!close()) {
			return false;
		}
	}

	hFile = fopen(filename.native().c_str(), "rb");

	return hFile != nullptr;
}

bool NativeFile::create(const boost::filesystem::path& filename) {
	if(hFile) {
		if(!close()) {
			return false;
		}
	}

	hFile = fopen(filename.native().c_str(), "wb");

	return hFile != nullptr;
}

bool NativeFile::close() {
	if(hFile) {
		if(fclose(hFile) != 0) {
			return false;
		}
	}

	hFile = nullptr;
	return true;
}

void NativeFile::Read(void* dest, size_t length) {
	if(hFile) {
		size_t len = fread(dest, 1U, length, hFile);

		if(len != length) {
			throw FileReadErrorException();
		}
	}

	return;
}

void NativeFile::Write(const void* buffer, size_t length) {
	if(hFile) {
		size_t len = fwrite(buffer, 1U, length, hFile);

		if(len != length) {
			throw FileWriteErrorException();
		}
	}

	return;
}

void NativeFile::Seek(long offset) {
	if(hFile) {
		if(fseek(hFile, offset, SEEK_CUR) != 0) {
			throw FileInvalidSeekException();
		}
	}
}

void NativeFile::SetPosition(size_t offset) {
	if(hFile) {
		if(fseek(hFile, static_cast<long>(offset), SEEK_SET) != 0) {
			throw FileInvalidSeekException();
		}
	}
}

size_t NativeFile::GetPosition() {
	if(hFile) {
		return static_cast<size_t>(ftell(hFile));
	}

	return 0;
}

size_t NativeFile::GetSize() {
	if(hFile) {
		auto currentPointer = ftell(hFile);
		fseek(hFile, 0, SEEK_END);
		auto fSize = ftell(hFile);
		fseek(hFile, currentPointer, SEEK_SET);
		return static_cast<size_t>(fSize);
	}

	return 0;
}

bool NativeFile::IsEndOfFile() {
	if(hFile) {
		return feof(hFile) != 0;
	}

	return true;
}
