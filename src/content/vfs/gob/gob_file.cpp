#include "gob_file.h"
#include "base/io/exception.h"

gorc::content::vfs::gob::gob_file::gob_file(const boost::filesystem::path& gobPath, size_t chunkOffset, size_t chunkLength)
    : file(gobPath, false), chunkOffset(chunkOffset),
      chunkLength(chunkLength), chunkEnd(chunkOffset + chunkLength) {
    file.set_position(chunkOffset);
    return;
}

void gorc::content::vfs::gob::gob_file::read(void* dest, size_t size) {
    if(position() + size > chunkLength) {
        throw io::file_read_exception();
    }

    file.read(dest, size);
}

void gorc::content::vfs::gob::gob_file::seek(int offset) {
    size_t currentPosition = position();
    if((offset < 0 && static_cast<size_t>(std::abs(offset)) > currentPosition)
            || currentPosition + offset >= chunkEnd) {
        throw io::file_seek_exception();
    }

    file.seek(offset);
}

void gorc::content::vfs::gob::gob_file::set_position(size_t offset) {
    if(offset > chunkLength) {
        throw io::file_seek_exception();
    }

    file.set_position(chunkOffset + offset);
}

size_t gorc::content::vfs::gob::gob_file::position() {
    return file.position() - chunkOffset;
}

size_t gorc::content::vfs::gob::gob_file::size() {
    return chunkLength;
}

bool gorc::content::vfs::gob::gob_file::at_end() {
    return position() >= chunkLength;
}
